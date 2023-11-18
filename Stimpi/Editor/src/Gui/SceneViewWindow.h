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

	private:
		void DrawUIComponents(ImVec2 winPos, ImVec2 winSize);
		bool PickUIComponents(ImVec2 pickPos);

		bool m_Show = true;
		ImGuiWindowFlags m_Flags;
		bool m_Hovered = false;
		bool m_Focused = false;
	};
}