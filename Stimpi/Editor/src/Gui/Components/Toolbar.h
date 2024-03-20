#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class Toolbar
	{
	public:

		static void Begin(const char* name);
		static void End();

		static bool ToolbarButton(const char* name);
		static void Separator();
	private:
		// Draw methods
		static void DrawBackground();

	};
}