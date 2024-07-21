#include "stpch.h"
#include "Stimpi/Core/WindowManager.h"

#include "Stimpi/Core/Runtime.h"
#include "Stimpi/Log.h"

#include "SDL_mouse.h"

namespace Stimpi
{
	/**
	 * WindowManager
	 */

	Stimpi::WindowManager* WindowManager::Instance()
	{
		static auto m_Instance = Create();
		return m_Instance.get();
	}

	std::unique_ptr<WindowManager> WindowManager::Create()
	{
		switch (Runtime::GetRuntimeMode())
		{
		case RuntimeMode::APPLICATION:	return std::make_unique<WindowManagerRuntime>();
		case RuntimeMode::EDITOR:		return std::make_unique<WindowManagerEditor>();
		}

		return std::make_unique<WindowManagerRuntime>();	// Actually we can never get here :P
	}

	void WindowManager::SetActiveWindow(Window* window)
	{
		m_Window = window;
	}


	/**
	 * WindowManagerEditor
	 */

	glm::vec2 WindowManagerEditor::GetWindowPosition()
	{
		if (m_WindowPositionGetter)
			return m_WindowPositionGetter();

		return { -1.f, -1.f };
	}

	glm::vec2 WindowManagerEditor::GetWindowSize()
	{
		if (m_WindowSizeGetter)
			return m_WindowSizeGetter();

		return { 0.0f, 0.0f };
	}

	glm::vec2 WindowManagerEditor::GetMouseWindowPostition()
	{
		if (m_MouseWindowPositionGetter)
			return m_MouseWindowPositionGetter();

		return { 0.0f, 0.0f };
	}

	void WindowManagerEditor::SetWindowPositionGetter(getter_function function)
	{
		m_WindowPositionGetter = function;
	}

	void WindowManagerEditor::SetWindowSizeGetter(getter_function function)
	{
		m_WindowSizeGetter = function;
	}


	void WindowManagerEditor::SetMouseWindowPositionGetter(getter_function function)
	{
		m_MouseWindowPositionGetter = function;
	}

	/**
	 * WindowManagerRuntime
	 */

	glm::vec2 WindowManagerRuntime::GetWindowPosition()
	{
		if (m_Window)
		{
			int x, y;
			m_Window->GetWindowPosition(&x, &y);
			return { x, y };
		}

		return { -1.f, -1.f };
	}

	glm::vec2 WindowManagerRuntime::GetWindowSize()
	{
		if (m_Window)
		{
			return { m_Window->GetWidth(), m_Window->GetHeight() };
		}

		return { 0.0f, 0.0f };
	}

	glm::vec2 WindowManagerRuntime::GetMouseWindowPostition()
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		return { x, y };
	}

}