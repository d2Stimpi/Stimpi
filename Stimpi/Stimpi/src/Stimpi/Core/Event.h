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
		BaseEvent(EventType type) : m_EventType(type), m_RawEvent(nullptr) {}
		virtual ~BaseEvent() {}

		virtual void LogEvent() = 0;

		void SetRawEvent(void* raw) { m_RawEvent = raw; }
		EventType GetEventType() { return m_EventType; }
		void* GetRawEvent() { return m_RawEvent; }
	private:
		EventType m_EventType;
		void* m_RawEvent; // For example SDLEvent type - for specific use not covered with SubTypes
	};

	/******************************************************************************************/
	/******************************** Events **************************************************/

	class ST_API KeyboardEvent : public BaseEvent
	{
	public:
		KeyboardEvent(KeyboardEventType type, uint32_t repeat, uint32_t keyCode) : BaseEvent(EventType::KeyboardEvent), m_Type(type), m_Repeat(repeat), m_KeyCode(keyCode){}
		~KeyboardEvent() { ST_CORE_TRACE("~KeyboardEvent: {0}, KeyCode: {1}", GetStringKeyboardEvent(m_Type), m_KeyCode); };
		
		void LogEvent() { ST_CORE_TRACE("KeyboardEvent: {0}, KeyCode: {1}", GetStringKeyboardEvent(m_Type), m_KeyCode); }

		static KeyboardEvent* CreateKeyboardEvent(SDL_Event e)
		{
			KeyboardEventType type{};
			uint32_t repeat = 0;

			switch (e.key.type)
			{
			case SDL_KEYDOWN:
				if (e.key.repeat != 0) 
				{
					type = KeyboardEventType::KEY_EVENT_REPEAT;
					repeat = e.key.repeat;
				}
				else
				{
					type = KeyboardEventType::KEY_EVENT_DOWN;
				}
				break;
			case SDL_KEYUP: 
				type = KeyboardEventType::KEY_EVENT_UP; 
				break;
			default:
				ST_CORE_WARN("Unprocessed Keyboard event: {0}", e.key.type);
				type = KeyboardEventType::NONE;
			}

			return new KeyboardEvent(type, repeat, e.key.keysym.scancode);
		}

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
		static MouseEvent* CreateMouseEvent(SDL_Event e)
		{
			MouseEventType type{};
			uint32_t x = 0;
			uint32_t y = 0;
			uint8_t button = 0;

			switch (e.key.type)
			{
				case SDL_MOUSEBUTTONDOWN: 
					type = MouseEventType::MOUSE_EVENT_BUTTONDOWN;
					break;
				case SDL_MOUSEBUTTONUP: 
					type = MouseEventType::MOUSE_EVENT_BUTTONUP; 
					break;
				case SDL_MOUSEWHEEL: 
					if (e.wheel.y > 0) 
					{
						type = MouseEventType::MOUSE_EVENT_WHEELUP;
					} 
					else if (e.wheel.y < 0) 
					{
						type = MouseEventType::MOUSE_EVENT_WHEELDOWN;
					} 
					else 
					{
						ST_CORE_WARN("Unprocessed Mouse event: {0}", e.key.type);
						type = MouseEventType::NONE;
					}
					break;
				case SDL_MOUSEMOTION: type = MouseEventType::MOUSE_EVENT_MOTION; break;
				default:
					ST_CORE_WARN("Unprocessed Mouse event: {0}", e.key.type);
					type = MouseEventType::NONE;
			}
			if ((e.key.type == SDL_MOUSEBUTTONDOWN) || (e.key.type == SDL_MOUSEBUTTONUP) || (e.key.type == SDL_MOUSEMOTION))
			{
				x = e.button.x;
				y = e.button.y;
			}
			if ((e.key.type == SDL_MOUSEBUTTONDOWN) || (e.key.type == SDL_MOUSEBUTTONUP))
			{
				button = e.button.button;
			}
			return new MouseEvent(type, x, y, button);
		}
	private:
		MouseEventType m_Type;
		uint32_t m_X;
		uint32_t m_Y;
		uint8_t m_Button;
	};

	class ST_API WindowEvent : public BaseEvent
	{
	public:
		WindowEvent(WindowEventType type) : BaseEvent(EventType::WindowEvent), m_Type(type) {}
		~WindowEvent() {}

		void LogEvent() { ST_CORE_TRACE("WindowEventType: {0}", GetStringWindowEvent(m_Type)); }

		static WindowEvent* CreateWindowEvnet(SDL_Event e)
		{
			WindowEventType type{};

			switch (e.window.event)
			{
			case SDL_WINDOWEVENT_CLOSE: type = WindowEventType::WINDOW_EVENT_QUIT; break;
			case SDL_WINDOWEVENT_RESIZED: type = WindowEventType::WINDOW_EVENT_RESIZE; break;
			default:
				ST_CORE_WARN("Unprocessed Window event: {0}", e.window.event);
				type = WindowEventType::NONE;
				break;
			}

			return new WindowEvent(type);
		}
	private:
		WindowEventType m_Type;
	};

	class EventFactory
	{
	public:
		static BaseEvent* EventCreate(SDL_Event e)
		{
			switch (GetEventType(e))
			{
			case EventType::KeyboardEvent: return KeyboardEvent::CreateKeyboardEvent(e);
			case EventType::MouseEvent: return MouseEvent::CreateMouseEvent(e);
			case EventType::WindowEvent: return WindowEvent::CreateWindowEvnet(e);
			default: ST_CORE_WARN("EventCreate: Unknow event!"); return nullptr;
			}
		}

		static EventType GetEventType(SDL_Event e)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				return EventType::KeyboardEvent;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
			case SDL_MOUSEMOTION:
				return EventType::MouseEvent;
			case SDL_WINDOWEVENT:
				return EventType::WindowEvent;
			default:
				return EventType::None;
				break;
			}
		}
	};

	class ST_API Event
	{
	public:
		Event() : m_Handled(false), m_Event({}) {};
		Event(SDL_Event e) : m_Handled(false), m_Event(e) {};
		virtual ~Event() {};

		void Handled() { m_Handled = true; }
		bool IsHandled() { return m_Handled; }
		void SetEvent(SDL_Event e) { m_Event = e; }

		inline const SDL_Event& GetRawEvent() { return m_Event; }
	private:
		bool m_Handled;
		// Temp encapsualtion of SDL event
		SDL_Event m_Event;
	};
}