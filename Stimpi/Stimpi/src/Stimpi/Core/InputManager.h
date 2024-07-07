#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Event.h"

#include "Stimpi/Core/KeyCodes.h"
#include "Stimpi/Core/MouseCodes.h"

/* 
* TODO: Mouse events
*  - Scroll Up/Down poll events
*/

namespace Stimpi
{
	class ST_API KeyboardEventHandler
	{
		using KeyboardOnEventFunction = std::function<bool(KeyboardEvent e)>;

	public:
		KeyboardEventHandler(KeyboardOnEventFunction onEvent) : m_OnEvent(onEvent) {};
		bool OnKeyboardEvent(KeyboardEvent e) { if (m_OnEvent != nullptr) { return m_OnEvent(e); } };
	private:
		KeyboardOnEventFunction m_OnEvent;
	};

	// Consider rewriting as Template

	class ST_API MouseEvnetHandler
	{
		using MouseOnEventFucntion = std::function<bool(MouseEvent e)>;

	public:
		MouseEvnetHandler(MouseOnEventFucntion onEvent) : m_OnEvent(onEvent) {};
		bool OnMouseEvent(MouseEvent e) { if (m_OnEvent != nullptr) { return m_OnEvent(e); } };
	private:
		MouseOnEventFucntion m_OnEvent;
	};

	class ST_API InputManager
	{
	public:
		static InputManager* Instance();

		void AddKeyboardEventHandler(KeyboardEventHandler* handler);
		void AddMouseEventHandler(MouseEvnetHandler* handler);

		void RemoveKeyboardEventHandler(KeyboardEventHandler* handler);
		void RemoveMouseEventHandler(MouseEvnetHandler* handler);
		
		bool HandleKeyboardEvent(KeyboardEvent event);
		bool HandleMouseEvent(MouseEvent event);

		static bool IsKeyDown(uint32_t keycode);
		static bool IsKeyPressed(uint32_t keycode);
		static bool IsKeyUp(uint32_t keycode);

		static bool IsMouseButtonDown(uint8_t mbt);
		static bool IsMouseButtonPressed(uint8_t mbt);
		static bool IsMouseButtonUp(uint8_t mbt);

		void AddEvent(KeyboardEvent event);
		void AddEvent(MouseEvent event);

		void ClearEvents();

	private:
		std::vector<KeyboardEventHandler*> m_keyboardEventHandlers;
		std::vector<KeyboardEvent> m_KeyboardEvents; // TODO: decide if really needed

		std::vector<MouseEvnetHandler*> m_mouseEventHandlers;
		std::vector<MouseEvent> m_MouseEvents;
	};
}