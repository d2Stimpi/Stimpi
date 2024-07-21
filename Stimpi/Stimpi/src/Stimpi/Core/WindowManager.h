#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Window.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	/**
	 * WindowManager base class
	 */
	class ST_API WindowManager
	{
	public:
		static WindowManager* Instance();
		static std::unique_ptr<WindowManager> Create();

		virtual glm::vec2 GetWindowPosition() = 0;
		virtual glm::vec2 GetWindowSize() = 0;
		virtual glm::vec2 GetMouseWindowPostition() = 0;

		void SetActiveWindow(Window* window);

	protected:
		Window* m_Window;
	};

	/**
	 *  WindowManager for Editor mode, allowing use of ImGui API for window stuff.
	 */
	class ST_API WindowManagerEditor : public WindowManager
	{
	public:
		using getter_function = std::function<glm::vec2(void)>;

		glm::vec2 GetWindowPosition() override;
		glm::vec2 GetWindowSize() override;
		glm::vec2 GetMouseWindowPostition() override;

		void SetWindowPositionGetter(getter_function function);
		void SetWindowSizeGetter(getter_function function);
		void SetMouseWindowPositionGetter(getter_function function);

	private:
		getter_function m_WindowPositionGetter = nullptr;
		getter_function m_WindowSizeGetter = nullptr;
		getter_function m_MouseWindowPositionGetter = nullptr;
	};

	/**
	 *	WindowManager for Runtime mode.
	 */
	class ST_API WindowManagerRuntime : public WindowManager
	{
	public:
		glm::vec2 GetWindowPosition() override;
		glm::vec2 GetWindowSize() override;
		glm::vec2 GetMouseWindowPostition() override;
	};
}