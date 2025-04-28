#pragma once

#include "ImGui/src/imgui.h"
#include "Stimpi/Core/Timestep.h"

namespace Stimpi
{
	class SceneViewWindow
	{
	public:
		SceneViewWindow();
		~SceneViewWindow();

		void OnImGuiRender();

		bool IsHovered() { return m_Hovered; }
		bool IsFocused() { return m_Focused; }

		ImVec2 GetWindowSize() { return m_WindowSize; }
		ImVec2 GetWindowPosition() { return m_WindowPosition; }
		ImVec2 GetMousePosition() { return m_MousePosition; }

	private:
		void DrawUIComponents(ImVec2 winPos, ImVec2 winSize);
		bool PickUIComponents(ImVec2 pickPos);

		ImVec2 GetWorldMousePosition();
	private:
		bool m_Show = true;
		ImGuiWindowFlags m_Flags;
		bool m_Hovered = false;
		bool m_Focused = false;

		ImVec2 m_WindowSize = { 0.0f, 0.0f };
		ImVec2 m_WindowPosition = { 0.0f, 0.0f };
		ImVec2 m_MousePosition = { 0.0f, 0.0f };
	};
}