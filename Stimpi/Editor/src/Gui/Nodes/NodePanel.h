#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class NodePanel
	{
	public:

		void OnImGuiRender();

		static void ShowWindow(bool show);
		static bool IsVisible();

	private:
	};
}