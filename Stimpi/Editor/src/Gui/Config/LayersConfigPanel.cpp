#include "stpch.h"
#include "Gui/Config/LayersConfigPanel.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Scene/SceneManager.h"

#include "Gui/Components/ImGuiEx.h"
#include "Gui/Components/UIPayload.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	bool LayersConfigPanel::m_Show = false;

	struct LayersConfigPanelContext
	{
		size_t m_Selected = 0;				// 0 - no selection
	};

	static LayersConfigPanelContext* s_Context;

	LayersConfigPanel::LayersConfigPanel()
	{
		s_Context = new LayersConfigPanelContext();
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
					bool mouseCaptured = false;
					bool itemHovered = false;

					auto& layers = Project::GetSortingLayers();
					for (size_t i = 0; i < layers.size(); i++)
					{
						auto& layer = layers[i];
						if (layer->m_Name.length() < 32)
							strcpy_s(layerNameInputBuff, layer->m_Name.c_str());

						bool selected = s_Context->m_Selected == i + 1;
						bool textDisabled = layer->m_Name == Project::GetDefaultSortingLayerName();
						std::string strID = fmt::format("##{}_{}", layer->m_Name, i);
						if (ImGuiEx::InputSelectable("Layer", strID.c_str(), layerNameInputBuff, sizeof(layerNameInputBuff), selected, textDisabled))
						{
							layer->m_Name = std::string(layerNameInputBuff);
						}

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !mouseCaptured)
						{
							if (ImGui::IsItemHovered())
							{
								mouseCaptured = true; // skip handling mouse click event for further Layer elements
								itemHovered = true;
								s_Context->m_Selected = i + 1;
							}
						}

						UIPayload::BeginSource("Payload_Layer", &i, sizeof(i), layer->m_Name.c_str());

						UIPayload::BeginTarget("Payload_Layer", [&i, &layers](void* data, uint32_t size) {
							size_t index = *(int*)data;

							// Moving down in sequence
							if (index < i)
							{
								for (size_t n = 0; n < i - index; n++)
								{
									std::swap(layers[index + n], layers[index + n + 1]);
								}
								s_Context->m_Selected = i + 1;
							}
							// Moving up in sequence
							else
							{
								for (size_t n = 0; n < index - i; n++)
								{
									std::swap(layers[index - n], layers[index - n - 1]);
								}
								s_Context->m_Selected = i + 1;
							}
						});
						
					}

					// Add/Remove Layer buttons
					ImGui::SetCursorPosX(ImGui::GetWindowContentRegionWidth() - 52);
					if (ImGui::Button("+##AddLayer", ImVec2(30.0f, 0)))
					{
						std::string newName = fmt::format("NewLayer_{}", layers.size());
						auto newLayer = std::make_shared<SortingLayer>(newName);
						layers.emplace_back(newLayer);
					}
					ImGui::SameLine(0.0f, 3.0f);
					if (ImGui::Button("-##RemoveLayer", ImVec2(30.0f, 0)))
					{
						if (s_Context->m_Selected != 0)
						{
							// Prevent removing default layer
							RemoveLayer(s_Context->m_Selected - 1);
						}
					}

					// Payload drop on remove button to delete layer
					UIPayload::BeginTarget("Payload_Layer", [this, &layers](void* data, uint32_t size) {
						size_t index = *(int*)data;
						RemoveLayer(index);
					});

					// IsItemHovered checks the remove button to avoid clearing selection index
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !itemHovered && !ImGui::IsItemHovered())
					{
						s_Context->m_Selected = 0;
					}
				}
				ImGui::End();
			}
		}
	}


	void LayersConfigPanel::RemoveLayer(const size_t index)
	{
		auto& layers = Project::GetSortingLayers();

		// Prevent removing default layer
		if (layers[index]->m_Name == Project::GetDefaultSortingLayerName())
		{
			ST_CORE_INFO("{} layer is not allowed to be removed (default layer)!", Project::GetDefaultSortingLayerName());
		}
		else
		{
			// Notify Scene to update SortingLayerComponents
			auto scene = SceneManager::Instance()->GetActiveScene();
			if (scene)
			{
				scene->OnSortingLayerRemove(layers[index ]->m_Name);
			}

			layers.erase(std::next(layers.begin(), index));

			// Clear selection after removal
			s_Context->m_Selected = 0;
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