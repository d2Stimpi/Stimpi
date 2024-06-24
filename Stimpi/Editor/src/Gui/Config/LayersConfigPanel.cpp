#include "stpch.h"
#include "Gui/Config/LayersConfigPanel.h"

#include "Stimpi/Log.h"
#include "Gui/Components/ImGuiEx.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	bool LayersConfigPanel::m_Show = false;

	enum class LayersConfigPanemMouseAction { NONE = 0, LAYER_HOVER, LAYER_DRAG };

	struct SortingLayer
	{
		std::string m_Name;

		SortingLayer() = default;
		SortingLayer(const SortingLayer&) = default;
		SortingLayer(std::string name)
			: m_Name(name)
		{}
	};

	struct LayersConfigPanelContext
	{
		LayersConfigPanemMouseAction m_MouseAction;

		std::vector<std::shared_ptr<SortingLayer>> m_Layers;
		int m_SelectedIndex = -1;
	};

	static LayersConfigPanelContext* s_Context;

	LayersConfigPanel::LayersConfigPanel()
	{
		s_Context = new LayersConfigPanelContext();

		s_Context->m_MouseAction = LayersConfigPanemMouseAction::NONE;

		// Test sample - TODO: hook to real data
		std::vector<std::string> testData = { "Layer 1", "Layer 2", "Layer 3", "Layer 4" };
		for (auto item : testData)
		{
			auto newLayer = std::make_shared<SortingLayer>(item);
			s_Context->m_Layers.emplace_back(newLayer);
		}
	}

	void LayersConfigPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			if (ImGui::Begin("Layers Config##Project", &m_Show)) // ImGuiWindowFlags_NoResize
			{
				if (ImGui::CollapsingHeader("Sorting Layers##Layers Config", ImGuiTreeNodeFlags_DefaultOpen))
				{
					char layerNameInputBuff[32] = { "Default" };

					for (int i = 0; i < s_Context->m_Layers.size(); i++)
					{
						auto layer = s_Context->m_Layers[i];
						ImGuiEx::InputSelectable(layer->m_Name.c_str(), layer->m_Name.c_str(), layerNameInputBuff, sizeof(layerNameInputBuff));

						if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
						{
							int n_next = i;
							if (ImGui::GetMouseDragDelta(0).y > 20.f)
							{
								n_next++;
								ImGui::ResetMouseDragDelta();
							}
							if (ImGui::GetMouseDragDelta(0).y < -20.f)
							{
								n_next--;
								ImGui::ResetMouseDragDelta();
							}
							if (n_next >= 0 && n_next < s_Context->m_Layers.size())
							{
								std::swap(s_Context->m_Layers[i], s_Context->m_Layers[n_next]);
								ST_CORE_INFO("Swap");
							}
						}
						
					}

					/*

					for (int n = 0; n < IM_ARRAYSIZE(sortingLayers); n++)
					{
						const char* item = sortingLayers[n];
						ImGui::Selectable(item, false, 0, ImVec2(ImGui::GetWindowContentRegionWidth() - 150.0f, 0.0f));

						if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
						{
							int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
							if (n_next >= 0 && n_next < IM_ARRAYSIZE(sortingLayers))
							{
								sortingLayers[n] = sortingLayers[n_next];
								sortingLayers[n_next] = item;
								ImGui::ResetMouseDragDelta();
							}
						}

						ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 150);
						std::string label = fmt::format("Layer##{}", item);
						if (ImGui::InputText(label.c_str(), layerNameInputBuff, sizeof(layerNameInputBuff), ImGuiInputTextFlags_EnterReturnsTrue))
						{

						}
					}*/
				}
				ImGui::End();
			}
		}
	}

	void LayersConfigPanel::ShowWindow(bool show)
	{
		m_Show = show;
	}

	bool LayersConfigPanel::IsVisible()
	{
		return m_Show;
	}

}