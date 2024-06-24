#include "stpch.h"
#include "Gui/Config/GraphicsConfigPanel.h"

#include "Stimpi/Core/Project.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	bool GraphicsConfigPanel::m_Show = false;

	void GraphicsConfigPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			if (ImGui::Begin("Graphics Config##Project", &m_Show)) // ImGuiWindowFlags_NoResize
			{
				const char* orderingAxis[] = { "None", "X", "Y", "Z" };
				const char* currentOrderingAxis = orderingAxis[(int)Project::GetGraphicsConfig().m_RenderingOrderAxis];

				ImGui::Text("Rendering Order Axis");
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 105);
				ImGui::SetNextItemWidth(100);
				if (ImGui::BeginCombo("##Rendering Order Axis", currentOrderingAxis))
				{
					for (int i = 0; i < IM_ARRAYSIZE(orderingAxis); i++)
					{
						bool isSelected = currentOrderingAxis == orderingAxis[i];
						if (ImGui::Selectable(orderingAxis[i], isSelected))
						{
							currentOrderingAxis = orderingAxis[i];
							auto config = Project::GetGraphicsConfig();
							config.m_RenderingOrderAxis = (RenderingOrderAxis)i;
							Project::SetGraphicsConfig(config);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				ImGui::End();
			}
		}

	}

	void GraphicsConfigPanel::ShowWindow(bool show)
	{
		m_Show = show;
	}

	bool GraphicsConfigPanel::IsVisible()
	{
		return m_Show;
	}

}