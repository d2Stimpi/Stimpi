#include "stpch.h"
#include "Gui/Components/SearchPopup.h"
#include "Gui/EditorUtils.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	struct Context
	{
		std::string m_SelectedItem = "";
		ImVec2 m_WinSize = { 200.0f, 160.0f };

		char m_SearchText[64];

		Context()
		{
			memset(m_SearchText, 0, sizeof(m_SearchText));
		}
	};

	static Context s_Context;

	bool SearchPopup::OnImGuiRender(const char* name, bool* show, std::vector<std::string>& data)
	{
		const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | \
							ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;
		bool itemSelected = false;

		ImGui::SetNextWindowSize(s_Context.m_WinSize);
		ImGui::Begin(name, nullptr, flags);

		ImGui::InputText("Search", s_Context.m_SearchText, sizeof(s_Context.m_SearchText));
		EditorUtils::SetActiveItemCaptureKeyboard(false);

		if (ImGui::Button("CLEAR", ImVec2(-1.0f, 0.0f)))
		{
			memset(s_Context.m_SearchText, 0, sizeof(s_Context.m_SearchText));
		}

		ImGui::Separator();
		ImGui::BeginChild("Data##SearchPopup");
		for (auto name : data)
		{
			std::string input = s_Context.m_SearchText;
			if (name.find(input) != -1 || input.empty())
			{
				if (ImGui::Selectable(name.c_str(), false))
				{
					s_Context.m_SelectedItem = name;

					// Close the window when item is selected
					*show = false;
					itemSelected = true;
				}
			}
		}
		
		ImGui::EndChild();

		// Check if click occurred outside of pop up and if so, close the window
		static bool ignoreFirstClick = true;
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			auto click = ImGui::GetMousePos();
			if (!EditorUtils::IsPositionInCurrentWindow(click) && ignoreFirstClick == false)
			{
				*show = false;
			}
		}
		ignoreFirstClick = false;
		// override the FirstClick flag when closing the window is confirmed
		if (*show == false)
			ignoreFirstClick = true;

		ImGui::End();

		return itemSelected;
	}

	std::string SearchPopup::GetSelection()
	{
		return s_Context.m_SelectedItem;
	}

}