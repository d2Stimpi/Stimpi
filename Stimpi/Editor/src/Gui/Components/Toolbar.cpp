#include "stpch.h"
#include "Gui/Components/Toolbar.h"

#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	struct ToolbarData
	{
		std::string m_Name;
		bool m_IsHovered = false;
		bool m_IsActive = true;

		ImVec2 m_Pos = { 0.0f, 0.0f };
		float m_RegionWidth = 0.0f;
	};

	struct ToolbarContext
	{
		std::vector<std::shared_ptr<ToolbarData>> m_Toolbars;

		ToolbarData* m_CurrentToolbar = nullptr;
		ImDrawList* m_DrawList = nullptr;

		std::vector<ToolbarStyle> m_StyleStack;
		ToolbarStyle m_CurrentStyle;
	};

	static ToolbarContext s_Context;


	static ToolbarData* FindToolbarByName(const char* name)
	{
		for (auto& toolbar : s_Context.m_Toolbars)
		{
			if (toolbar->m_Name == name)
			{
				return toolbar.get();
			}
		}

		return nullptr;
	}

	static ToolbarData* CreateNewToolbar(const char* name)
	{
		auto newToolbar = std::make_shared<ToolbarData>();
		newToolbar->m_Name = name;

		s_Context.m_Toolbars.emplace_back(newToolbar);
		// return ptr of last (new) vectors element
		return s_Context.m_Toolbars[s_Context.m_Toolbars.size() - 1].get();
	}

	void Toolbar::Begin(const char* name)
	{
		IM_ASSERT(name != NULL && name[0] != '\0');

		ToolbarData* toolbar = FindToolbarByName(name);
		if (toolbar == nullptr)
			toolbar = CreateNewToolbar(name);
		s_Context.m_CurrentToolbar = toolbar;

		// DrawToolbar
		s_Context.m_DrawList = ImGui::GetWindowDrawList();
		DrawBackground();
	}

	void Toolbar::End()
	{
		s_Context.m_CurrentToolbar = nullptr;
		// toolbar can keep ref on active window to where it belongs
		// TODO: housekeeping
	}

	void Toolbar::DrawBackground()
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 windowSize = ImGui::GetContentRegionAvail();

		s_Context.m_CurrentToolbar->m_Pos = cursorPos;
		s_Context.m_CurrentToolbar->m_RegionWidth = windowSize.x;

		ImVec2 min = cursorPos;
		ImVec2 max = {min.x + windowSize.x, min.y + s_Context.m_CurrentStyle.m_Size.y};

		s_Context.m_DrawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));

		std::string btnName = fmt::format("{}##InvButton", s_Context.m_CurrentToolbar->m_Name);

		ImGui::SetCursorScreenPos({ cursorPos.x, cursorPos.y + s_Context.m_CurrentStyle.m_Size.y + 3 });
	}

	bool Toolbar::ToolbarButton(const char* name)
	{
		IM_ASSERT_USER_ERROR(s_Context.m_CurrentToolbar != nullptr, "Toolbar Begin() not called!");

		bool ret = false;
		// Save current cursor position
		ImVec2 cursor = ImGui::GetCursorScreenPos();

		ImGui::SetCursorScreenPos(s_Context.m_CurrentToolbar->m_Pos);
		ret = ImGui::Button(name, { 0.0f, s_Context.m_CurrentStyle.m_Size.x });
		ImVec2 btnSize = ImGui::GetItemRectSize();

		// Restore saved cursor position
		ImGui::SetCursorScreenPos(cursor);
		// Updated local cursor position for next toolbar element
		s_Context.m_CurrentToolbar->m_Pos.x += btnSize.x;
		return ret;
	}

	bool Toolbar::ToolbarIconButton(const char* name, const char* iconName, ImVec2 size /*= { -1.0f, -1.0f }*/)
	{
		IM_ASSERT_USER_ERROR(s_Context.m_CurrentToolbar != nullptr, "Toolbar Begin() not called!");

		bool ret = false;
		// Save current cursor position
		ImVec2 cursor = ImGui::GetCursorScreenPos();

		// If size param was not used, use style size
		ImVec2 buttonSize = s_Context.m_CurrentStyle.m_Size;
		if (size.x != -1.0f && size.y != -1.0f)
			buttonSize = size;

		// Push toolbar style over to ImGuiEx style
		ImGuiExStyle exStyle;
		exStyle.m_IconSize = s_Context.m_CurrentStyle.m_IconSize;
		
		ImGui::SetCursorScreenPos(s_Context.m_CurrentToolbar->m_Pos);
		ImGuiEx::PushStyle(exStyle);
		ret = ImGuiEx::IconButton(name, iconName, buttonSize);
		ImGuiEx::PopStyle();
		ImVec2 btnSize = ImGui::GetItemRectSize();

		// Restore saved cursor position
		ImGui::SetCursorScreenPos(cursor);
		// Updated local cursor position for next toolbar element
		s_Context.m_CurrentToolbar->m_Pos.x += btnSize.x;
		return ret;
	}

	bool Toolbar::ToolbarToggleIconButton(const char* name, const char* iconName, bool active, ImVec2 size /*= { -1.0f, -1.0f }*/)
	{
		IM_ASSERT_USER_ERROR(s_Context.m_CurrentToolbar != nullptr, "Toolbar Begin() not called!");

		bool ret = false;
		ImVec2 cursor = ImGui::GetCursorScreenPos();

		// If size param was not used, use style size
		ImVec2 buttonSize = s_Context.m_CurrentStyle.m_Size;
		if (size.x != -1.0f && size.y != -1.0f)
			buttonSize = size;

		// Push toolbar style over to ImGuiEx style
		ImGuiExStyle exStyle;
		exStyle.m_IconSize = s_Context.m_CurrentStyle.m_IconSize;

		ImGui::SetCursorScreenPos(s_Context.m_CurrentToolbar->m_Pos);
		ImGuiEx::PushStyle(exStyle);
		ret = ImGuiEx::ToggleIconButton(name, iconName, active, buttonSize);
		ImGuiEx::PopStyle();
		ImVec2 btnSize = ImGui::GetItemRectSize();

		ImGui::SetCursorScreenPos(cursor);
		// Updated local cursor position
		s_Context.m_CurrentToolbar->m_Pos.x += btnSize.x;
		return ret;
	}

	void Toolbar::Separator()
	{
		ImVec2 min = s_Context.m_CurrentToolbar->m_Pos;
		ImVec2 max = { min.x + s_Context.m_CurrentStyle.m_SeparatorWidth, min.y + s_Context.m_CurrentStyle.m_Size.x };

		s_Context.m_DrawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));

		// Updated local cursor position
		s_Context.m_CurrentToolbar->m_Pos.x += s_Context.m_CurrentStyle.m_SeparatorWidth;
	}

	void Toolbar::SetButtonTooltip(const char* tooltip)
	{
		ImGui::SetItemTooltip(tooltip);
	}

	Stimpi::ToolbarStyle Toolbar::GetStyle()
	{
		return s_Context.m_CurrentStyle;
	}

	// TODO: assert check for Push/Pop pair consistency
	void Toolbar::PushStyle(ToolbarStyle style)
	{
		s_Context.m_StyleStack.push_back(s_Context.m_CurrentStyle);
		s_Context.m_CurrentStyle = style;
	}

	void Toolbar::PopStyle()
	{
		if (s_Context.m_StyleStack.size() > 0)
		{
			s_Context.m_CurrentStyle = s_Context.m_StyleStack.back();
			s_Context.m_StyleStack.pop_back();
		}
	}

}