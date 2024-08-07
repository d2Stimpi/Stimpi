#pragma once

#include "Stimpi/Graphics/Texture.h"
#include "Gui/Utils/EditorResources.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	struct ImGuiExStyle
	{
		ImVec2 m_IconButtonSize = { 13.0f, 13.0f };
		float m_IconOffset = 17.0f;	// Button width + 4

		ImGuiExStyle() = default;
	};

	class ImGuiEx
	{
	public:
		static bool IconButton(const char* strID, std::string iconName);
		static void Icon(std::string iconName);

		static bool InputSelectable(const char* label, const char* strID, char* buf, size_t buf_size, bool selected = false);

		static ImGuiExStyle& GetStyle();
	};
}