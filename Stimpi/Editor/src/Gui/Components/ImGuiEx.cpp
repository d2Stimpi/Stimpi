#include "stpch.h"
#include "Gui/Components/ImGuiEx.h"

#include "Gui/EditorUtils.h"
#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

// TODO: combine Toolbar and ImGuiEx styles

namespace Stimpi
{
	ImGuiExStyle s_Style;
	std::vector<ImGuiExStyle> s_StyleStack;

	ImGuiExStyle& ImGuiEx::GetStyle()
	{
		return s_Style;
	}

	// TODO: assert check for Push/Pop pair consistency
	void ImGuiEx::PushStyle(ImGuiExStyle style)
	{
		s_StyleStack.push_back(s_Style);
		s_Style = style;
	}

	void ImGuiEx::PopStyle()
	{
		if (s_StyleStack.size() > 0)
		{
			s_Style = s_StyleStack.back();
			s_StyleStack.pop_back();
		}
	}

	void ImGuiEx::SetCurrentLineTextOffset()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		auto& style = ImGui::GetStyle();
		window->DC.CurrLineTextBaseOffset = style.FramePadding.y;
	}

	bool ImGuiEx::IconButton(const char* strID, std::string iconName, ImVec2 size)
	{
		bool retVal = false;
		auto& style = ImGui::GetStyle();

		ImVec2 cursor = ImGui::GetCursorScreenPos();

		retVal = ImGui::Button(strID, size);

		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImVec2 icon_pos_min = { cursor.x + size.x / 2.0f - s_Style.m_IconSize.x / 2.0f, cursor.y + size.y / 2.0f - s_Style.m_IconSize.y / 2.0f };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_IconSize.x, icon_pos_min.y + s_Style.m_IconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)iconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		return retVal;
	}

	bool ImGuiEx::ToggleIconButton(const char* strID, std::string iconName, bool active, ImVec2 size /*= s_DefaultIconButtonSize*/)
	{
		bool retVal = false;
		auto& style = ImGui::GetStyle();

		ImVec2 cursor = ImGui::GetCursorScreenPos();
		
		// active color 66 150 250
		if (active)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(34, 110, 190, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(26, 100, 190, 255));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(66, 150, 250, 255));
		}
		retVal = ImGui::Button(strID, size);
		if (active)
		{ 
			ImGui::PopStyleColor(3);
		}

		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			ImVec2 icon_pos_min = { cursor.x + size.x / 2.0f - s_Style.m_IconSize.x / 2.0f, cursor.y + size.y / 2.0f - s_Style.m_IconSize.y / 2.0f };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_IconSize.x, icon_pos_min.y + s_Style.m_IconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)iconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		return retVal;
	}

	void ImGuiEx::Icon(std::string iconName)
	{
		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture)
		{
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			ImGui::Image((void*)(intptr_t)iconTexture->GetTextureID(), s_Style.m_IconButtonSize, uv_min, uv_max);
		}
	}

	bool ImGuiEx::InputSelectable(const char* label, const char* strID, char* buf, size_t bufSize, bool selected, bool textDisabled)
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

		if (textDisabled)
			ImGui::BeginDisabled();
		ImGui::Text(label);
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 150);
		retVal = ImGui::InputText(strID, buf, bufSize, ImGuiInputTextFlags_EnterReturnsTrue);
		if (textDisabled)
			ImGui::EndDisabled();

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

		auto& style = ImGui::GetStyle();
		ImVec2 cursor = ImGui::GetCursorScreenPos();

		ret = ImGui::InputText(strID, buffer, bufferSize, ImGuiInputTextFlags_OffsetInputByIcon);
		EditorUtils::SetActiveItemCaptureKeyboard(false);
		bool isHovered = ImGui::IsItemHovered();
		bool isActive = ImGui::IsItemActive();

		ImVec2 windowSize = ImGui::GetContentRegionAvail();
		ImVec2 inputRectSize = ImGui::GetItemRectSize();

		Texture* iconTexture = EditorResources::GetIconTexture(EDITOR_ICON_SEARCH);
		if (iconTexture)
		{
			ImVec2 icon_pos_min = { cursor.x + 5, cursor.y + style.FramePadding.y };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_IconSize.x, icon_pos_min.y + s_Style.m_IconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)iconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		if (strlen(buffer) == 0)
		{
			auto color = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TextDisabled]);

			ImVec2 note_text_pos = { cursor.x + style.SmallIconPadding + style.ItemSpacing.x, cursor.y + style.FramePadding.y };
			if (isHovered)
				color = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

			if (!isActive)
				window->DrawList->AddText(note_text_pos, color, hintTxt);
		}

		// Clear text button
		if (strlen(buffer) != 0)
		{
			// For button behavior
			static bool held = false;
			bool released = false;

			ImGuiContext& g = *GImGui;
			ImVec2 center = ImVec2(cursor.x + inputRectSize.x - s_Style.m_IconSize.x + 3.0f, cursor.y + inputRectSize.y / 2.0f);
			float radius = inputRectSize.y / 2.0f - 2.0f;
			bool hovered = ImGui::IsMouseHoveringRect(ImVec2(center.x - radius, center.y - radius), ImVec2(center.x + radius, center.y + radius));

			if (hovered)
			{
				held = ImGui::IsMouseDown(ImGuiMouseButton_Left);
				if (held)
				{
					ImGui::ClearActiveID();	// We have to lose active state (focus) to be able to clear buffer data
				}
				released = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
				ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
				ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonActive);
				window->DrawList->AddCircleFilled(center, ImMax(2.0f, g.FontSize * 0.5f + 1.0f), col);
			}

			ImU32 cross_col = ImGui::GetColorU32(ImGuiCol_Text);
			float cross_extent = g.FontSize * 0.5f * 0.7071f - 1.0f;
			center = ImVec2(center.x - 0.5f, center.y - 0.5f);
			window->DrawList->AddLine(ImVec2(center.x + cross_extent, center.y + cross_extent), ImVec2(center.x - cross_extent, center.y - cross_extent), cross_col, 1.0f);
			window->DrawList->AddLine(ImVec2(center.x + cross_extent, center.y - cross_extent), ImVec2(center.x - cross_extent, center.y + cross_extent), cross_col, 1.0f);

			// Button press and release action
			if ((held && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) || released)
			{
				memset(buffer, 0, bufferSize);
			}
		}

		return ret;
	}

	bool ImGuiEx::TreeNodeIcon(const void* ptrID, ImGuiTreeNodeFlags flags, const std::string& label, const std::string& iconName)
	{
		bool ret = false;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture)
			ret = ImGui::TreeNodeBehavior(window->GetID(ptrID), flags, label.c_str(), NULL, (void*)(intptr_t)iconTexture->GetTextureID());
		else
			ret = ImGui::TreeNodeBehavior(window->GetID(ptrID), flags, label.c_str());

		return ret;
	}

	bool ImGuiEx::TreeNodeHeaderIcon(const void* ptrID, ImGuiTreeNodeFlags flags, const std::string& label, const std::string& iconName)
	{
		bool ret = false;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		flags = flags | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowOverlap;

		Texture* iconTexture = EditorResources::GetIconTexture(iconName);
		if (iconTexture)
			ret = ImGui::TreeNodeBehavior(window->GetID(ptrID), flags, label.c_str(), NULL, (void*)(intptr_t)iconTexture->GetTextureID());
		else
			ret = ImGui::TreeNodeBehavior(window->GetID(ptrID), flags, label.c_str());

		return ret;
	}

	bool ImGuiEx::PrefabHeaderIcon(const std::string& label)
	{
		ImGuiStyle& style = ImGui::GetStyle();
		bool ret = false;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		// Save cursor position
		ImVec2 invisibleBtnPos = ImGui::GetCursorPos();
		const ImVec2 label_size = ImGui::CalcTextSize(label.c_str(), NULL, true);
		const ImVec2 frame_size = ImVec2(ImGui::GetWindowContentRegionWidth(), label_size.y + style.FramePadding.y * 2.0f);
		const ImVec2 btn_size = ImVec2(24.0f, frame_size.y);

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImRect bb = { pos.x + 1, pos.y,  pos.x + frame_size.x,  pos.y + frame_size.y };
		ImRect btn_bb = { pos.x + 1, pos.y,  pos.x + btn_size.x,  pos.y + btn_size.y };
		
		// Rendering frame and highlight
		const ImU32 col = ImGui::GetColorU32(ImGuiCol_HeaderActive);
		ImGui::RenderFrame(bb.Min, bb.Max, col, false, style.FrameRounding);
		if (ImGui::IsMouseHoveringRect(btn_bb.Min, btn_bb.Max))
			ImGui::RenderFrame(btn_bb.Min, btn_bb.Max, ImGui::GetColorU32(ImGuiCol_HeaderHovered), false, style.FrameRounding);

		ImGui::SetCursorPos(invisibleBtnPos);
		if (ImGui::InvisibleButton("PrefabInvisBtn", btn_size, ImGuiButtonFlags_None))
		{
			ret = true;
		}

		Texture* backIconTexture = EditorResources::GetIconTexture(EDITOR_ICON_NAV_BACK);
		if (backIconTexture)
		{
			ImVec2 icon_pos_min = { btn_bb.Min.x + btn_size.x / 6.0f, btn_bb.Min.y + style.FramePadding.y };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_IconSize.x, icon_pos_min.y + s_Style.m_IconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)backIconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		float text_offset = (frame_size.x - label_size.x) / 2.0f;
		float icon_offset = (frame_size.x - label_size.x) / 2.0f - s_Style.m_IconSize.x - style.FramePadding.x;

		Texture* iconTexture = EditorResources::GetIconTexture(EDITOR_ICON_BCUBE);
		if (iconTexture)
		{
			ImVec2 icon_pos_min = { pos.x + icon_offset, pos.y + style.FramePadding.y };
			ImVec2 icon_pos_max = { icon_pos_min.x + s_Style.m_IconSize.x, icon_pos_min.y + s_Style.m_IconSize.y };
			window->DrawList->AddImage((void*)(intptr_t)iconTexture->GetTextureID(), icon_pos_min, icon_pos_max, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), ImGui::ColorConvertFloat4ToU32(ImVec4(0.785f, 0.785f, 0.785f, 1.0f)));
		}

		ImGui::SameLine(text_offset);
		// Set line size and text offset (if not using framed style)
		window->DC.CurrLineSize.y = 21.0f;
		window->DC.CurrLineTextBaseOffset = 3.0f;	// This is used to force an offset on text position
													// Otherwise it won't be well centered on the current line

		ImGui::Text(label.c_str());

		return ret;
	}

}