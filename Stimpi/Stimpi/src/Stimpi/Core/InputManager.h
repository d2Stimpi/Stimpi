#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/KeyCodes.h"
#include "Stimpi/Core/Event.h"

/* 
* TODO: Mouse events
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

	class ST_API InputManager
	{
	public:
		static InputManager* Instance();

		void AddKeyboardEventHandler(KeyboardEventHandler* handler);
		void RemoveKeyboardEventHandler(KeyboardEventHandler* handler);
		bool HandleKeyboardEvent(KeyboardEvent event);

		static bool IsKeyPressed(uint32_t keycode);

		void AddEvent(KeyboardEvent event);
		void ClearEvents();

	private:
		std::vector<KeyboardEventHandler*> m_keyboardEventHandlers;
		std::vector<KeyboardEvent> m_KeyboardEvents; // TODO: decide if realy needed
	};
}