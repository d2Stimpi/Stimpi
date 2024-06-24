#include "stpch.h"
#include "Gui/Components/ImGuiEx.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	ImGuiExStyle s_Style;

	bool ImGuiEx::IconButton(const char* strID, std::string iconName)
	{
		bool retVal = false;
		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture->Loaded())
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			retVal = ImGui::ImageButton(strID, (void*)(intptr_t)iconTexture->GetTextureID(), s_Style.m_IconButtonSize, uv_min, uv_max);
		}

		return retVal;
	}

	void ImGuiEx::Icon(std::string iconName)
	{
		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture->Loaded())
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)iconTexture->GetTextureID(), s_Style.m_IconButtonSize, uv_min, uv_max);
		}
	}

	bool ImGuiEx::InputSelectable(const char* label, const char* strID, char* buf, size_t bufSize, bool selected)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		bool retVal = false;

		// Save cursor position
		ImVec2 invisibleBtnPos = ImGui::GetCursorPos();

		ImGui::Text(label);
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 150);
		std::string str = fmt::format("##{}", strID);
		retVal = ImGui::InputText(str.c_str(), buf, bufSize, ImGuiInputTextFlags_EnterReturnsTrue);

		std::string btnID = fmt::format("##InvisibleButton{}", strID);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true); 
		const ImVec2 frame_size = ImVec2(ImGui::GetWindowContentRegionWidth(), label_size.y + style.FramePadding.y * 2.0f);

		// Invisible button is used to enable IsItemActive and IsItemHovered on this "item"
		// Restore cursor position to draw button
		ImGui::SetCursorPos(invisibleBtnPos);
		if (ImGui::InvisibleButton(btnID.c_str(), frame_size, ImGuiButtonFlags_None))
		{
			ST_CORE_INFO("Clicked invis button {}", strID);
		}

		return retVal;
	}

	Stimpi::ImGuiExStyle& ImGuiEx::GetStyle()
	{
		return s_Style;
	}

}