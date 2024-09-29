#pragma once

#include "Stimpi/Graphics/Texture.h"
#include "Gui/Utils/EditorResources.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	struct ImGuiExStyle
	{
		ImVec2 m_IconButtonSize = s_DefaultIconButtonSize;
		ImVec2 m_SmallIconSize = s_DefaultSmallIconSize;
		float m_IconOffset = 17.0f;	// Button width + 4

		ImGuiExStyle() = default;
	};

	class ImGuiEx
	{
	public:
		static ImGuiExStyle& GetStyle();
		static void PushStyle(ImGuiExStyle style);
		static void PopStyle();

		static bool IconButton(const char* strID, std::string iconName, ImVec2 size = s_DefaultIconButtonSize);
		static void Icon(std::string iconName);
		static bool InputSelectable(const char* label, const char* strID, char* buf, size_t bufSize, bool selected = false, bool textDisabled = false);
		static bool SearchInput(const char* strID, char* buffer, size_t bufferSize, const char* hintTxt = "Search");
	};
}