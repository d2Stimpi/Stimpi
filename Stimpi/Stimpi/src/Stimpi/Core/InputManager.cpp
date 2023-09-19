#include "Stimpi/Core/InputManager.h"

namespace Stimpi
{
	KeyboardEventType s_KeyStates[ST_NUM_KEYCODES];

	InputManager* InputManager::Instance()
	{
		static auto m_Instance = std::make_unique<InputManager>();
		return m_Instance.get();
	}

	void InputManager::AddKeyboardEventHandler(KeyboardEventHandler* handler)
	{
		if (handler != nullptr)
		{
			m_keyboardEventHandlers.push_back(handler);
		}
	}

	void InputManager::RemoveKeyboardEventHandler(KeyboardEventHandler* handler)
	{
		if (handler != nullptr)
		{
			m_keyboardEventHandlers.erase(std::remove(m_keyboardEventHandlers.begin(), m_keyboardEventHandlers.end(), handler));
		}
	}

	bool InputManager::HandleKeyboardEvent(KeyboardEvent event)
	{
		bool handled = false;

		for (auto handler : m_keyboardEventHandlers)
		{
			handled = handler->OnKeyboardEvent(event);
			if (handled == true)
				break; // Consume the event
		}

		return handled;
	}

	bool InputManager::IsKeyPressed(uint32_t keycode)
	{
		return s_KeyStates[keycode] == KeyboardEventType::KEY_EVENT_DOWN;
	}

	void InputManager::AddEvent(KeyboardEvent event)
	{
		m_KeyboardEvents.push_back(event);
		// Update KeyState data
		if ((event.GetType() == KeyboardEventType::KEY_EVENT_DOWN) || (event.GetType() == KeyboardEventType::KEY_EVENT_UP))
		{
			s_KeyStates[event.GetKeyCode()] = event.GetType();
		}
	}

	void InputManager::ClearEvents()
	{
		m_KeyboardEvents.clear();
	}
}