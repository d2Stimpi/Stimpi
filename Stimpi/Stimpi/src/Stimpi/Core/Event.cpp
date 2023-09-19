#include "Event.h"

#include <functional>
#include <vector>
#include <memory>

#define ST_EVENT_DBG false

namespace Stimpi
{
	/******************************************************************************************/
	/******************************** KeyboardEvent *******************************************/

	KeyboardEvent* KeyboardEvent::CreateKeyboardEvent(SDL_Event e)
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
			if (ST_EVENT_DBG) ST_CORE_WARN("Unprocessed Keyboard event: {0}", e.key.type);
			type = KeyboardEventType::NONE;
		}

		return new KeyboardEvent(type, repeat, e.key.keysym.scancode);
	}


	/******************************************************************************************/
	/******************************** MouseEvent **********************************************/

	MouseEvent* MouseEvent::CreateMouseEvent(SDL_Event e)
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
				if(ST_EVENT_DBG) ST_CORE_WARN("Unprocessed Mouse event: {0}", e.key.type);
				type = MouseEventType::NONE;
			}
			break;
		case SDL_MOUSEMOTION: type = MouseEventType::MOUSE_EVENT_MOTION; break;
		default:
			if (ST_EVENT_DBG) ST_CORE_WARN("Unprocessed Mouse event: {0}", e.key.type);
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


	/******************************************************************************************/
	/******************************** WindowEvent *********************************************/

	WindowEvent* WindowEvent::CreateWindowEvnet(SDL_Event e)
	{
		WindowEventType type{};
		uint32_t width = 0;
		uint32_t height = 0;

		switch (e.window.event)
		{
		case SDL_WINDOWEVENT_CLOSE: type = WindowEventType::WINDOW_EVENT_QUIT; break;
		case SDL_WINDOWEVENT_RESIZED: 
			type = WindowEventType::WINDOW_EVENT_RESIZE;
			width = e.window.data1;
			height = e.window.data2;
			break;
		default:
			if (ST_EVENT_DBG) ST_CORE_WARN("Unprocessed Window event: {0}", e.window.event);
			type = WindowEventType::NONE;
			break;
		}

		return new WindowEvent(type, width, height);
	}

	/******************************************************************************************/
	/******************************** EventFactory ********************************************/

	BaseEvent* EventFactory::EventCreate(SDL_Event e)
	{
		switch (GetEventType(e))
		{
		case EventType::KeyboardEvent:  return KeyboardEvent::CreateKeyboardEvent(e);
		case EventType::MouseEvent:		return MouseEvent::CreateMouseEvent(e);
		case EventType::WindowEvent:	return WindowEvent::CreateWindowEvnet(e);
		default: if (ST_EVENT_DBG) ST_CORE_WARN("EventCreate: Unknow event!"); return nullptr;
		}
	}

	EventType EventFactory::GetEventType(SDL_Event e)
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
}