#include "stpch.h"
#include "Gui/Components/SearchPopup.h"

#include "Gui/EditorUtils.h"
#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	struct Context
	{
		bool m_Active = false;
		std::string m_SelectedItem = "";
		ImVec2 m_WinSize = { 200.0f, 160.0f };

		char m_SearchText[64];

		Context()
		{
			memset(m_SearchText, 0, sizeof(m_SearchText));
		}
	};

	static Context s_Context;

	bool SearchPopup::OnImGuiRender(const char* popupName, std::vector<std::string>& data)
	{
		bool itemSelected = false;

		ImGui::SetNextWindowSize(s_Context.m_WinSize);
		s_Context.m_Active = ImGui::BeginPopup(popupName);
		if (s_Context.m_Active)
		{
			ImGui::InputText("Search", s_Context.m_SearchText, sizeof(s_Context.m_SearchText));
			EditorUtils::SetActiveItemCaptureKeyboard(false);

			if (ImGui::Button("CLEAR", ImVec2(-1.0f, 0.0f)))
			{
				memset(s_Context.m_SearchText, 0, sizeof(s_Context.m_SearchText));
			}

			ImGui::Separator();
			ImGui::BeginChild("##GlobalSearchPopup");
			for (auto name : data)
			{
				std::string input = s_Context.m_SearchText;
				if (name.find(input) != -1 || input.empty())
				{
					// Check for special data strings
					if (name._Starts_with("#"))
					{
						ImGui::SeparatorText(name.c_str() + 1);
						continue;
					}

					if (ImGui::Selectable(name.c_str(), false))
					{
						s_Context.m_SelectedItem = name;

						// Close the window when item is selected
						itemSelected = true;
					}
				}
			}
			ImGui::EndChild();
			ImGui::EndPopup();
		}

		return itemSelected;
	}

	std::string SearchPopup::GetSelection()
	{
		return s_Context.m_SelectedItem;
	}

	void SearchPopup::OpenPopup(const char* popupName)
	{
		ImGui::OpenPopup(popupName);
	}

	void SearchPopup::ClosePopup()
	{
		ImGui::CloseCurrentPopup();
	}

	bool SearchPopup::IsActive()
	{
		return s_Context.m_Active;
	}

}