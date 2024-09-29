#pragma once

#include "ImGui/src/imgui.h"
#include "Gui/Components/ImGuiEx.h"

namespace Stimpi
{
	struct ToolbarStyle
	{
		float m_Height = 35.0f;
		float m_SeparatorWidth = 2.0f;
	};

	class Toolbar
	{
	public:

		static void Begin(const char* name);
		static void End();

		static bool ToolbarButton(const char* name);
		static bool ToolbarIconButton(const char* name, const char* iconName, float width = s_DefaultIconButtonSize.x);
		static void Separator();

		static ToolbarStyle GetStyle();
		static void PushStyle(ToolbarStyle style);
		static void PopStyle();

	private:
		// Draw methods
		static void DrawBackground();

	};
}