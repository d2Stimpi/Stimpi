#pragma once

#include "ImGui/src/imgui.h"

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
		bool m_Show = true;
		ImGuiWindowFlags m_Flags;
		bool m_Hovered = false;
		bool m_Focused = false;
	};
}