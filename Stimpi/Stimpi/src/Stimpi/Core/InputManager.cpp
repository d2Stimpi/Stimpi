#include "stpch.h"
#include "Stimpi/Core/InputManager.h"

namespace Stimpi
{
	KeyboardEventType s_KeyStates[ST_NUM_KEYCODES];
	MouseEventType s_MouseButtonStates[ST_NUM_MOUSE_CODES];

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

	void InputManager::AddMouseEventHandler(MouseEvnetHandler* handler)
	{
		if (handler != nullptr)
		{
			m_mouseEventHandlers.push_back(handler);
		}
	}

	void InputManager::RemoveKeyboardEventHandler(KeyboardEventHandler* handler)
	{
		if (handler != nullptr)
		{
			m_keyboardEventHandlers.erase(std::remove(m_keyboardEventHandlers.begin(), m_keyboardEventHandlers.end(), handler));
		}
	}

	void InputManager::RemoveMouseEventHandler(MouseEvnetHandler* handler)
	{
		if (handler != nullptr)
		{
			if (m_mouseEventHandlers.size() > 0)
				m_mouseEventHandlers.erase(std::remove(m_mouseEventHandlers.begin(), m_mouseEventHandlers.end(), handler));
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

	bool InputManager::HandleMouseEvent(MouseEvent event)
	{
		bool handled = false;

		for (auto handler : m_mouseEventHandlers)
		{
			handled = handler->OnMouseEvent(event);
			if (handled == true)
				break; // Consume the event
		}

		return handled;
	}

	bool InputManager::IsKeyPressed(uint32_t keycode)
	{
		return s_KeyStates[keycode] == KeyboardEventType::KEY_EVENT_DOWN;
	}

	bool InputManager::IsMouseButtonPressed(uint8_t mbt)
	{
		return s_MouseButtonStates[mbt] == MouseEventType::MOUSE_EVENT_BUTTONDOWN;
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

	void InputManager::AddEvent(MouseEvent event)
	{
		m_MouseEvents.push_back(event);
		if ((event.GetType() == MouseEventType::MOUSE_EVENT_BUTTONDOWN) || (event.GetType() == MouseEventType::MOUSE_EVENT_BUTTONUP))
		{
			s_MouseButtonStates[event.GetButton()] = event.GetType();
		}
	}

	void InputManager::ClearEvents()
	{
		m_KeyboardEvents.clear();
		m_MouseEvents.clear();
	}
}