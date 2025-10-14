#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class PrefabInspectWindow
	{
	public:
		PrefabInspectWindow();
		~PrefabInspectWindow();

		void OnImGuiRender();

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
		static bool m_Show;
	};
}