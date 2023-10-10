#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class SceneViewWindow
	{
	public:
		SceneViewWindow();
		~SceneViewWindow();

		void Draw();
	private:
		bool m_Show = true;
		ImGuiWindowFlags m_Flags;
	};
}