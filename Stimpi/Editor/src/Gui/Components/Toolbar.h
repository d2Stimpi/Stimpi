#pragma once

#include "ImGui/src/imgui.h"
#include "Gui/Components/ImGuiEx.h"

namespace Stimpi
{
	struct ToolbarStyle
	{
		float m_SeparatorWidth = 2.0f;
		ImVec2 m_Size = { 35.0f, 21.0f };
		ImVec2 m_IconSize = s_DefaultIconButtonSize;
	};

	class Toolbar
	{
	public:

		static void Begin(const char* name);
		static void End();

		static bool ToolbarButton(const char* name);
		static bool ToolbarIconButton(const char* name, const char* iconName, ImVec2 size = { -1.0f, -1.0f });
		static bool ToolbarToggleIconButton(const char* name, const char* iconName, bool active, ImVec2 size = { -1.0f, -1.0f });
		static void Separator();
		static void SetButtonTooltip(const char* tooltip);

		static ToolbarStyle GetStyle();
		static void PushStyle(ToolbarStyle style);
		static void PopStyle();

	private:
		// Draw methods
		static void DrawBackground();

	};
}