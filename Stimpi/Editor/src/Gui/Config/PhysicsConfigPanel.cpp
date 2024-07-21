#include "stpch.h"
#include "Gui/Config/PhysicsConfigPanel.h"

#include "Stimpi/Core/Project.h"

#include <glm/gtc/type_ptr.hpp>
#include "ImGui/src/imgui.h"

namespace Stimpi
{
	bool PhysicsConfigPanel::m_Show = false;

	void PhysicsConfigPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			if (ImGui::Begin("Physics Config##Project", &m_Show)) // ImGuiWindowFlags_NoResize
			{
				PhysicsConfig& physicsConfig = Project::GetPhysicsConfig();

				ImGui::Text("Gravity force value");
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 105);
				ImGui::SetNextItemWidth(100);
				ImGui::DragFloat2("##GravityForce_Config", glm::value_ptr(physicsConfig.m_GravityForce));

				ImGui::End();
			}
		}
	}

	void PhysicsConfigPanel::ShowWindow(bool show)
	{
		m_Show = show;
	}

	bool PhysicsConfigPanel::IsVisible()
	{
		return m_Show;
	}

}