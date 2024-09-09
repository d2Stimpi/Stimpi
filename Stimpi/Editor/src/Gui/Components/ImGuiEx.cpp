#include "stpch.h"
#include "Gui/Components/ImGuiEx.h"

#include "Gui/EditorUtils.h"
#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	ImGuiExStyle s_Style;

	ImGuiExStyle& ImGuiEx::GetStyle()
	{
		return s_Style;
	}

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
		std::string btnID = fmt::format("##InvisibleButton{}", strID);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true); 
		const ImVec2 frame_size = ImVec2(ImGui::GetWindowContentRegionWidth(), label_size.y + style.FramePadding.y * 2.0f);

		// Rendering highlight
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImRect bb = { pos.x - 3, pos.y - 1,  pos.x + frame_size.x + 6,  pos.y + frame_size.y + 1 };
		if (ImGui::IsMouseHoveringRect(bb.Min, bb.Max) || selected)
		{
			const ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
			ImGui::RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
		}

		ImGui::Text(label);
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 150);
		retVal = ImGui::InputText(strID, buf, bufSize, ImGuiInputTextFlags_EnterReturnsTrue);
		EditorUtils::SetActiveItemCaptureKeyboard(false);

		// Invisible button is used to enable Drag and Drop, IsItemActive and IsItemHovered on this "item"
		// Restore cursor position to draw button
		ImGui::SetCursorPos(invisibleBtnPos);
		ImGui::InvisibleButton(btnID.c_str(), frame_size, ImGuiButtonFlags_None);

		return retVal;
	}

	bool ImGuiEx::SearchInput(const char* strID, char* buffer, size_t bufferSize, const char* hintTxt)
	{
		bool ret = false;

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
		ret = ImGui::InputText(strID, buffer, bufferSize, ImGuiInputTextFlags_OffsetInputByIcon);
		bool isHovered = ImGui::IsItemHovered();
		bool isActive = ImGui::IsItemActive();

		ImVec2 cursor = ImGui::GetCursorScreenPos();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		Texture* iconTexture = EditorResources::GetIconTexture(EDITOR_ICON_SEARCH);
		if (iconTexture->Loaded())
		{
			ImVec2 icon_pos_min = { cursor.x + 5, cursor.y - s_Style.m_IconOffset - 5 };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_SmallIconSize.x, icon_pos_min.y + s_Style.m_SmallIconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)iconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		if (strlen(buffer) == 0)
		{
			auto& style = ImGui::GetStyle();
			auto color = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TextDisabled]);

			ImVec2 note_text_pos = { cursor.x + style.SmallIconPadding + style.ItemSpacing.x, cursor.y - s_Style.m_IconOffset - 5 };
			if (isHovered)
				color = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

			if (!isActive)
				window->DrawList->AddText(note_text_pos, color, hintTxt);
		}

		return ret;
	}

}