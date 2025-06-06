#pragma once

#include <SDL.h>
#include <string>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Log.h"

#include "Stimpi/Physics/Physics.h"
#include "Stimpi/Utils/PlatformUtils.h"

namespace Stimpi
{
	enum class EventType {None = 0, WindowEvent, KeyboardEvent, MouseEvent, PhysicsEvent, SystemShellEvent, UnknownEvent };
	enum class KeyboardEventType {NONE = 0, KEY_EVENT_DOWN, KEY_EVENT_UP, KEY_EVENT_REPEAT };
	enum class MouseEventType { NONE = 0, MOUSE_EVENT_BUTTONDOWN, MOUSE_EVENT_BUTTONUP, MOUSE_EVENT_BUTTONHOLD, MOUSE_EVENT_WHEELUP, MOUSE_EVENT_WHEELDOWN, MOUSE_EVENT_MOTION };
	enum class WindowEventType { NONE = 0, WINDOW_EVENT_QUIT, WINDOW_EVENT_RESIZE};
	enum class PhysicsEventType { NONE = 0, COLLISION_BEGIN, COLLISION_END, COLLISION_PRESOLVE, COLLISION_POSTSOLVE };
	enum class SystemShellEventType { NONE = 0, SH_CREATED, SH_UPDATED, SH_DELETED, SH_RENAMED };

	/* Helper dbg stringify functions */
	std::string GetStringKeyboardEvent(KeyboardEventType e);
	std::string GetStringMouseEvent(MouseEventType e);
	std::string GetStringWindowEvent(WindowEventType e);
	std::string GetStringPhysicsEvent(PhysicsEventType e);
	std::string GetStringSystemShellEvent(SystemShellEventType e);

	class ST_API Event
	{
	public:
		Event(EventType type) : m_EventType(type), m_RawSDLEvent(), m_Handled(false) {}
		virtual ~Event() {}

		virtual void LogEvent() = 0;

		void Handled() { m_Handled = true; }
		bool IsHandled() { return m_Handled; }

		void SetRawSDLEvent(SDL_Event raw) { m_RawSDLEvent = raw; }
		SDL_Event* GetRawSDLEvent() { return &m_RawSDLEvent; }
		EventType GetEventType() { return m_EventType; }
	private:
		EventType m_EventType;
		SDL_Event m_RawSDLEvent; // For example SDLEvent type - for specific use not covered with SubTypes
		bool m_Handled;
	};

	/******************************************************************************************/
	/******************************** Events **************************************************/

	class ST_API KeyboardEvent : public Event
	{
	public:
		KeyboardEvent(KeyboardEventType type, uint32_t repeat, uint32_t keyCode) : Event(EventType::KeyboardEvent), m_Type(type), m_Repeat(repeat), m_KeyCode(keyCode){}
		~KeyboardEvent() { /*ST_CORE_TRACE("~KeyboardEvent: {0}, KeyCode: {1}", GetStringKeyboardEvent(m_Type), m_KeyCode);*/ };
		
		void LogEvent() { ST_CORE_TRACE("KeyboardEvent: {0}, KeyCode: {1}", GetStringKeyboardEvent(m_Type), m_KeyCode); }

		static KeyboardEvent* CreateKeyboardEvent(SDL_Event e);

		KeyboardEventType GetType() { return m_Type; }
		uint32_t GetRepeat() { return m_Repeat; }
		uint32_t GetKeyCode() { return m_KeyCode; }
		static EventType GetStaticType() { return EventType::KeyboardEvent; }
	private:
		KeyboardEventType m_Type;
		uint32_t m_Repeat;
		uint32_t m_KeyCode;
	};

	class ST_API MouseEvent : public Event
	{
	public:
		MouseEvent(MouseEventType type, uint32_t x, uint32_t y, uint8_t button, float scrollx, float scrolly) 
			: Event(EventType::MouseEvent), m_Type(type), m_X(x), m_Y(y), m_Button(button), m_ScrollX(scrollx), m_ScrollY(scrolly) {}
		~MouseEvent() {}

		void LogEvent() 
		{
			if (m_Type == MouseEventType::MOUSE_EVENT_BUTTONDOWN || m_Type == MouseEventType::MOUSE_EVENT_BUTTONUP)
				ST_CORE_TRACE("MouseEventType: {0}, Button: {1}, Coords: {2},{3}", GetStringMouseEvent(m_Type), m_Button, m_X, m_Y);
			else if((m_Type == MouseEventType::MOUSE_EVENT_WHEELUP) || (m_Type == MouseEventType::MOUSE_EVENT_WHEELDOWN))
				ST_CORE_TRACE("MouseEventType: {0} , Scroll: {1}, {2}", GetStringMouseEvent(m_Type), m_ScrollX, m_ScrollY);
			else if(m_Type == MouseEventType::MOUSE_EVENT_MOTION)
				ST_CORE_TRACE("MouseEventType: {0}, Coords: {1},{2}", GetStringMouseEvent(m_Type), m_X, m_Y);
			else
				ST_CORE_TRACE("MouseEventType: {0}", GetStringMouseEvent(m_Type));
		}

		// TODO: confirm event type before creating it
		static MouseEvent* CreateMouseEvent(SDL_Event e);

		MouseEventType GetType() { return m_Type; }
		uint32_t GetX() { return m_X; }
		uint32_t GetY() { return m_Y; }
		uint8_t GetButton() { return m_Button; }
		float GetScrollX() { return m_ScrollX; }
		float GetScrollY() { return m_ScrollY; }
		static EventType GetStaticType() { return EventType::MouseEvent; }
	private:
		MouseEventType m_Type;
		uint32_t m_X;
		uint32_t m_Y;
		uint8_t m_Button;
		float m_ScrollX;
		float m_ScrollY;
	};

	class ST_API WindowEvent : public Event
	{
	public:
		WindowEvent(WindowEventType type, uint32_t width, uint32_t height)
			: Event(EventType::WindowEvent), m_Type(type), m_Width(width), m_Height(height) {}
		~WindowEvent() {}

		void LogEvent() { ST_CORE_TRACE("WindowEventType: {0}", GetStringWindowEvent(m_Type)); }

		static WindowEvent* CreateWindowEvent(SDL_Event e);

		WindowEventType GetType() { return m_Type; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		static EventType GetStaticType() { return EventType::WindowEvent; }
	private:
		WindowEventType m_Type;
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class ST_API PhysicsEvent : public Event
	{
	public:
		PhysicsEvent()
			: Event(EventType::PhysicsEvent), m_Type(PhysicsEventType::NONE), m_Collision({})
		{}
		PhysicsEvent(PhysicsEventType type, Collision collision)
			: Event(EventType::PhysicsEvent), m_Type(type), m_Collision(collision)
		{}
		~PhysicsEvent() {}

		void LogEvent() { ST_CORE_TRACE("PhysicsEventType: {0}", GetStringPhysicsEvent(m_Type)); }

		static PhysicsEvent* CreatePhysicsEvent(PhysicsEventType type, Collision collision);

		PhysicsEventType GetType() { return m_Type; }
		Collision& GetCollisionData() { return m_Collision; }

		static EventType GetStaticType() { return EventType::PhysicsEvent; }
	private:
		PhysicsEventType m_Type;
		Collision m_Collision;
	};

	class ST_API SystemShellEvent : public Event
	{
	public:
		SystemShellEvent()
			: Event(EventType::SystemShellEvent), m_Type(SystemShellEventType::NONE), m_FilePath({}), m_NewFilePath({})
		{}
		SystemShellEvent(SystemShellEventType type, const std::string& filePath, const std::string& newFilePath)
			: Event(EventType::SystemShellEvent), m_Type(type), m_FilePath(filePath), m_NewFilePath(newFilePath)
		{}
		~SystemShellEvent() {}

		void LogEvent() { ST_CORE_TRACE("SystemShellEvent: {0}", GetStringSystemShellEvent(m_Type)); }

		static SystemShellEvent* CreateSystemShellEvent(ShellEvetData eventData);

		SystemShellEventType GetType() { return m_Type; }
		std::string GetFilePath() { return m_FilePath; }
		std::string GetNewFilePath() { return m_NewFilePath; }

		static EventType GetStaticType() { return EventType::SystemShellEvent; }
	private:
		SystemShellEventType m_Type;
		std::string m_FilePath;
		std::string m_NewFilePath;	// For SH_RENAMED event
	};

	/* Used to enable passing unprocessed Raw SDLEvnets to ImGui */
	class ST_API UnknownEvent : public Event
	{
	public:
		UnknownEvent() : Event(EventType::UnknownEvent) {}
		~UnknownEvent() {}

		void LogEvent() { /*ST_CORE_TRACE("UnknownEvent");*/ }

		static UnknownEvent* CreateUnknownEvent(SDL_Event e);
	private:
	};

	class EventFactory
	{
	public:
		static Event* EventCreate(SDL_Event e);
		static EventType GetEventType(SDL_Event e);
	};

	// Mainly used as filter of events based on sub-type 
	template <typename T>
	class ST_API EventDispatcher
	{
		using EventFunc = std::function<bool(T*)>;
	public:

		void Dispatch(Event* event, EventFunc func)
		{
			if (event->GetEventType() == T::GetStaticType())
			{
				bool handled = false;
				handled = func((T*)event);
				if (handled)
					event->Handled();
			}
		}
	};
}