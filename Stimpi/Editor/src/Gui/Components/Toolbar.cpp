#include "stpch.h"
#include "Gui/Components/Toolbar.h"

#include "Stimpi/Log.h"
#include "Gui/Components/ImGuiEx.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	struct ToolbarStyle
	{
		float m_Height = 35.0f;
		float m_SeparatorWidth = 2.0f;
	};

	static ToolbarStyle s_Style;

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
		ImVec2 max = {min.x + windowSize.x, min.y + s_Style.m_Height}; // TODO: style it!

		s_Context.m_DrawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_FrameBg)));

		std::string btnName = fmt::format("{}##InvButton", s_Context.m_CurrentToolbar->m_Name);
		/*ImGui::InvisibleButton(btnName.c_str(), { windowSize.x, s_Style.m_Height }, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		s_Context.m_CurrentToolbar->m_IsHovered = ImGui::IsItemHovered(); // Hovered
		s_Context.m_CurrentToolbar->m_IsActive = ImGui::IsItemActive();   // Held*/

		ImGui::SetCursorScreenPos({ cursorPos.x, cursorPos.y + s_Style.m_Height + 3 });
	}

	bool Toolbar::ToolbarButton(const char* name)
	{
		IM_ASSERT_USER_ERROR(s_Context.m_CurrentToolbar != nullptr, "Toolbar Begin() not called!");

		bool ret = false;
		ImVec2 cursor = ImGui::GetCursorScreenPos();

		ImGui::SetCursorScreenPos(s_Context.m_CurrentToolbar->m_Pos);
		ret = ImGui::Button(name, { 0.0f, s_Style.m_Height });
		ImVec2 btnSize = ImGui::GetItemRectSize();

		ImGui::SetCursorScreenPos(cursor);
		// Updated local cursor position
		s_Context.m_CurrentToolbar->m_Pos.x += btnSize.x;
		return ret;
	}

	void Toolbar::Separator()
	{
		ImVec2 min = s_Context.m_CurrentToolbar->m_Pos;
		ImVec2 max = { min.x + s_Style.m_SeparatorWidth, min.y + s_Style.m_Height };

		s_Context.m_DrawList->AddRectFilled(min, max, ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_WindowBg)));

		// Updated local cursor position
		s_Context.m_CurrentToolbar->m_Pos.x += s_Style.m_SeparatorWidth;
	}

}