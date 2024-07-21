#include "stpch.h"
#include "Stimpi/Core/InputManager.h"

#include "Stimpi/Core/WindowManager.h"

namespace Stimpi
{
	KeyboardEventType s_KeyStates[ST_NUM_KEYCODES];
	KeyboardEventType s_KeyStatesInstant[ST_NUM_KEYCODES];	// Event data lasts only 1 frame, for detecting ON_DOWN and ON_UP
	MouseEventType s_MouseButtonStates[ST_NUM_MOUSE_CODES];
	MouseEventType s_MouseButtonStatesInstant[ST_NUM_MOUSE_CODES];

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

	bool InputManager::IsKeyDown(uint32_t keycode)
	{
		return s_KeyStatesInstant[keycode] == KeyboardEventType::KEY_EVENT_DOWN;
	}

	bool InputManager::IsKeyPressed(uint32_t keycode)
	{
		return s_KeyStates[keycode] == KeyboardEventType::KEY_EVENT_DOWN;
	}

	bool InputManager::IsKeyUp(uint32_t keycode)
	{
		return s_KeyStatesInstant[keycode] == KeyboardEventType::KEY_EVENT_UP;
	}

	bool InputManager::IsMouseButtonDown(uint8_t mbt)
	{
		return s_MouseButtonStatesInstant[mbt] == MouseEventType::MOUSE_EVENT_BUTTONDOWN;
	}

	bool InputManager::IsMouseButtonPressed(uint8_t mbt)
	{
		return s_MouseButtonStates[mbt] == MouseEventType::MOUSE_EVENT_BUTTONDOWN;
	}

	bool InputManager::IsMouseButtonUp(uint8_t mbt)
	{
		return s_MouseButtonStatesInstant[mbt] == MouseEventType::MOUSE_EVENT_BUTTONUP;
	}

	glm::vec2 InputManager::GetMousePosition()
	{
		return WindowManager::Instance()->GetMouseWindowPostition();
		//return m_MouseCursorPosition;
	}

	void InputManager::AddEvent(KeyboardEvent event)
	{
		m_KeyboardEvents.push_back(event);
		// Update KeyState data
		if ((event.GetType() == KeyboardEventType::KEY_EVENT_DOWN) ||
			(event.GetType() == KeyboardEventType::KEY_EVENT_UP))
		{
			s_KeyStates[event.GetKeyCode()] = event.GetType();
			s_KeyStatesInstant[event.GetKeyCode()] = event.GetType();
		}

		//ST_CORE_INFO("Input: Keyboard event - {}", GetStringKeyboardEvent(event.GetType()));
	}

	void InputManager::AddEvent(MouseEvent event)
	{
		m_MouseEvents.push_back(event);
		if ((event.GetType() == MouseEventType::MOUSE_EVENT_BUTTONDOWN) ||
			(event.GetType() == MouseEventType::MOUSE_EVENT_BUTTONUP))
		{
			s_MouseButtonStates[event.GetButton()] = event.GetType();
			s_MouseButtonStatesInstant[event.GetButton()] = event.GetType();
		}

		if (event.GetType() == MouseEventType::MOUSE_EVENT_MOTION)
		{
			m_MouseCursorPosition = { event.GetX(), event.GetY() };
		}

		//ST_CORE_INFO("Input: Mouse event - {}", GetStringMouseEvent(event.GetType()));
	}

	void InputManager::ClearEvents()
	{
		// Reset key states if no active keys
		for (auto& event : m_KeyboardEvents)
		{
			if ((s_KeyStatesInstant[event.GetKeyCode()] == KeyboardEventType::KEY_EVENT_DOWN) ||
				(s_KeyStatesInstant[event.GetKeyCode()] == KeyboardEventType::KEY_EVENT_UP))
				s_KeyStatesInstant[event.GetKeyCode()] = KeyboardEventType::NONE;
		}

		// Reset button states if no active buttons
		for (auto& event : m_MouseEvents)
		{
			if ((s_MouseButtonStatesInstant[event.GetButton()] == MouseEventType::MOUSE_EVENT_BUTTONDOWN) ||
				(s_MouseButtonStatesInstant[event.GetButton()] == MouseEventType::MOUSE_EVENT_BUTTONUP))
				s_MouseButtonStatesInstant[event.GetButton()] = MouseEventType::NONE;
		}

		m_KeyboardEvents.clear();
		m_MouseEvents.clear();
	}
}