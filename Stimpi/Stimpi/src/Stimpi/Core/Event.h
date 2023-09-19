#pragma once

#include <SDL.h>
#include <string>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Log.h"

namespace Stimpi
{
	enum class EventType {None = 0, WindowEvent, KeyboardEvent, MouseEvent };
	enum class KeyboardEventType {NONE = 0, KEY_EVENT_DOWN, KEY_EVENT_UP, KEY_EVENT_REPEAT };
	enum class MouseEventType { NONE = 0, MOUSE_EVENT_BUTTONDOWN, MOUSE_EVENT_BUTTONUP, MOUSE_EVENT_BUTTONHOLD, MOUSE_EVENT_WHEELUP, MOUSE_EVENT_WHEELDOWN, MOUSE_EVENT_MOTION };
	enum class WindowEventType { NONE = 0, WINDOW_EVENT_QUIT, WINDOW_EVENT_RESIZE};

	/* Helper dbg stringify functions */
	static std::string GetStringKeyboardEvent(KeyboardEventType e)
	{
		switch (e)
		{
			case KeyboardEventType::NONE:				return std::string("NONE");
			case KeyboardEventType::KEY_EVENT_DOWN:		return std::string("KEY_EVENT_DOWN");
			case KeyboardEventType::KEY_EVENT_UP:		return std::string("KEY_EVENT_UP");
			case KeyboardEventType::KEY_EVENT_REPEAT:	return std::string("KEY_EVENT_REPEAT");
			default: return std::string("NONE");
		}
	}

	static std::string GetStringMouseEvent(MouseEventType e)
	{
		switch (e)
		{
		case MouseEventType::NONE:						return std::string("NONE");
		case MouseEventType::MOUSE_EVENT_BUTTONDOWN:	return std::string("MOUSE_EVENT_BUTTONDOWN");
		case MouseEventType::MOUSE_EVENT_BUTTONUP:		return std::string("MOUSE_EVENT_BUTTONUP");
		case MouseEventType::MOUSE_EVENT_BUTTONHOLD:	return std::string("MOUSE_EVENT_BUTTONHOLD");
		case MouseEventType::MOUSE_EVENT_WHEELUP:		return std::string("MOUSE_EVENT_WHEELUP");
		case MouseEventType::MOUSE_EVENT_WHEELDOWN:		return std::string("MOUSE_EVENT_WHEELDOWN");
		case MouseEventType::MOUSE_EVENT_MOTION:		return std::string("MOUSE_EVENT_MOTION");
		default: return std::string("NONE");
		}
	}

	static std::string GetStringWindowEvent(WindowEventType e)
	{
		switch (e)
		{
		case WindowEventType::NONE:					return std::string("NONE");
		case WindowEventType::WINDOW_EVENT_QUIT:	return std::string("WINDOW_EVENT_QUIT");
		case WindowEventType::WINDOW_EVENT_RESIZE:	return std::string("WINDOW_EVENT_RESIZE");
		default: return std::string("NONE");
		}
	}

	class ST_API BaseEvent
	{
	public:
		BaseEvent(EventType type) : m_EventType(type), m_RawSDLEvent(), m_Handled(false) {}
		virtual ~BaseEvent() {}

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

	class ST_API KeyboardEvent : public BaseEvent
	{
	public:
		KeyboardEvent(KeyboardEventType type, uint32_t repeat, uint32_t keyCode) : BaseEvent(EventType::KeyboardEvent), m_Type(type), m_Repeat(repeat), m_KeyCode(keyCode){}
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

	class ST_API MouseEvent : public BaseEvent
	{
	public:
		MouseEvent(MouseEventType type, uint32_t x, uint32_t y, uint8_t button) : BaseEvent(EventType::MouseEvent), m_Type(type), m_X(x), m_Y(y), m_Button(button) {}
		~MouseEvent() {}

		void LogEvent() 
		{
			if (m_Type == MouseEventType::MOUSE_EVENT_BUTTONDOWN || m_Type == MouseEventType::MOUSE_EVENT_BUTTONUP)
				ST_CORE_TRACE("MouseEventType: {0}, Button: {1}, Coords: {2},{3}", GetStringMouseEvent(m_Type), m_Button, m_X, m_Y);
			else if(m_Type == MouseEventType::MOUSE_EVENT_WHEELUP)
				ST_CORE_TRACE("MouseEventType: {0}", GetStringMouseEvent(m_Type));
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
		static EventType GetStaticType() { return EventType::MouseEvent; }
	private:
		MouseEventType m_Type;
		uint32_t m_X;
		uint32_t m_Y;
		uint8_t m_Button;
	};

	class ST_API WindowEvent : public BaseEvent
	{
	public:
		WindowEvent(WindowEventType type, uint32_t width, uint32_t height) : BaseEvent(EventType::WindowEvent), m_Type(type), m_Width(width), m_Height(height) {}
		~WindowEvent() {}

		void LogEvent() { ST_CORE_TRACE("WindowEventType: {0}", GetStringWindowEvent(m_Type)); }

		static WindowEvent* CreateWindowEvnet(SDL_Event e);

		WindowEventType GetType() { return m_Type; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		static EventType GetStaticType() { return EventType::WindowEvent; }
	private:
		WindowEventType m_Type;
		uint32_t m_Width;
		uint32_t m_Height;
	};

	class EventFactory
	{
	public:
		static BaseEvent* EventCreate(SDL_Event e);
		static EventType GetEventType(SDL_Event e);
	};

	// Mainly used as filter of events based on sub-type 
	template <typename T>
	class EventDispatcher
	{
		using EventFunc = std::function<bool(T*)>;
	public:

		void Dispatch(BaseEvent* event, EventFunc func)
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