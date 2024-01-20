#include "stpch.h"
#include "Event.h"

#include <SDL_syswm.h>
#include <ShlObj_core.h>

#define ST_EVENT_DBG false
#define ST_EVENT_DBG_UNKNOWN_ONLY false

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
		float scroll_x = 0.0f;
		float scroll_y = 0.0f;

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
		if (e.key.type == SDL_MOUSEWHEEL)
		{
			scroll_x = e.wheel.preciseX;
			scroll_y = e.wheel.preciseY;
		}

		return new MouseEvent(type, x, y, button, scroll_x, scroll_y);
	}


	/******************************************************************************************/
	/******************************** WindowEvent *********************************************/

	WindowEvent* WindowEvent::CreateWindowEvent(SDL_Event e)
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
	/******************************** PhysicsEvent ********************************************/

	PhysicsEvent* PhysicsEvent::CreatePhysicsEvent(PhysicsEventType type, Collision collision)
	{
		return new PhysicsEvent(type, collision);
	}

	/******************************************************************************************/
	/******************************** SystemShellEvent ****************************************/

	Stimpi::SystemShellEvent* SystemShellEvent::CreateSystemShellEvent(ShellEvetData eventData)
	{
		SystemShellEventType type = SystemShellEventType::NONE;
		std::string filePath = eventData.m_FilePath;
		std::string newFilePath = eventData.m_NewFilePath;

		switch (eventData.m_Event)
		{
		case SHCNE_UPDATEITEM: 
			type = SystemShellEventType::SH_UPDATED;
			break;
		case SHCNE_CREATE:
			type = SystemShellEventType::SH_CREATED;
			break;
		case SHCNE_DELETE:
			type = SystemShellEventType::SH_DELETED;
			break;
		case SHCNE_RENAMEITEM:
			type = SystemShellEventType::SH_RENAMED;
			break;
		default:
			break;
		}

		return new SystemShellEvent(type, filePath, newFilePath);
	}

	/******************************************************************************************/
	/******************************** UnknowEvent *********************************************/

	UnknownEvent* UnknownEvent::CreateUnknownEvent(SDL_Event e)
	{
		return new UnknownEvent();
	}

	/******************************************************************************************/
	/******************************** EventFactory ********************************************/

	//TODO: rename to SDL Event Factory
	Event* EventFactory::EventCreate(SDL_Event e)
	{
		switch (GetEventType(e))
		{
		case EventType::KeyboardEvent:  return KeyboardEvent::CreateKeyboardEvent(e);
		case EventType::MouseEvent:		return MouseEvent::CreateMouseEvent(e);
		case EventType::WindowEvent:	return WindowEvent::CreateWindowEvent(e);
		default: if (ST_EVENT_DBG || ST_EVENT_DBG_UNKNOWN_ONLY) ST_CORE_WARN("EventCreate: Unknow event: {}", e.type); return UnknownEvent::CreateUnknownEvent(e);
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
		}
	}
}