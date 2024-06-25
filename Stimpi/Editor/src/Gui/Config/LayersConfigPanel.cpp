#include "stpch.h"
#include "Gui/Config/LayersConfigPanel.h"

#include "Stimpi/Log.h"
#include "Gui/Components/ImGuiEx.h"
#include "Gui/Components/UIPayload.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	bool LayersConfigPanel::m_Show = false;

	// Temp
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
		std::vector<std::shared_ptr<SortingLayer>> m_Layers;
	};

	static LayersConfigPanelContext* s_Context;

	LayersConfigPanel::LayersConfigPanel()
	{
		s_Context = new LayersConfigPanelContext();

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

					for (size_t i = 0; i < s_Context->m_Layers.size(); i++)
					{
						auto layer = s_Context->m_Layers[i];
						if (layer->m_Name.length() < 32)
							strcpy_s(layerNameInputBuff, layer->m_Name.c_str());

						if (ImGuiEx::InputSelectable("Layer", layer->m_Name.c_str(), layerNameInputBuff, sizeof(layerNameInputBuff)))
						{
							layer->m_Name = std::string(layerNameInputBuff);
						}

						UIPayload::BeginSource("Payload_Layer", &i, sizeof(i), layer->m_Name.c_str());

						UIPayload::BeginTarget("Payload_Layer", [&i](void* data, uint32_t size) {
							size_t index = *(int*)data;
							ST_CORE_INFO("Payload_Layer dropped: {}", index);

							// Moving down in sequence
							if (index < i)
							{
								for (size_t n = 0; n < i - index; n++)
								{
									std::swap(s_Context->m_Layers[index + n], s_Context->m_Layers[index + n + 1]);
								}
							}
							// Moving up in sequence
							else
							{
								for (size_t n = 0; n < index - i; n++)
								{
									std::swap(s_Context->m_Layers[index - n], s_Context->m_Layers[index - n - 1]);
								}
							}
							});
						
					}
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