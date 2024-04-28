#include "stpch.h"
#include "Gui/Config/SceneConfig.h"

#include "Stimpi/Core/Time.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Physics/Physics.h"
#include "Stimpi/Scene/Scene.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{

	SceneConfig::SceneConfig()
	{

	}

	SceneConfig::~SceneConfig()
	{

	}

	void SceneConfig::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::Begin("Scene Config", &m_Show);

			if (ImGui::CollapsingHeader("Graphics##Scene Config", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Resolution");
				const char* resolution_items[] = { "1920x1080", "1280x720" };
				static int current_resolution = 1;
				const float combo_width = ImGui::GetWindowWidth() * 0.80f;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::Combo("##Resolution", &current_resolution, resolution_items, IM_ARRAYSIZE(resolution_items)))
				{
					ST_CORE_INFO("Combo - Resolution: Secleted index {0}", current_resolution);
					//Resize Scene Canvas
					if (current_resolution == 0) Renderer2D::Instance()->ResizeCanvas(1920, 1080);
					if (current_resolution == 1) Renderer2D::Instance()->ResizeCanvas(1280, 720);
				};

				ImGui::Separator();
				ImGui::Text("Application FPS");
				const char* fps_items[] = { "60fps", "48fps", "24fps" };
				static int current_fps = 0;
				ImGui::SetNextItemWidth(combo_width);
				if (ImGui::Combo("##FPS", &current_fps, fps_items, 3))
				{
					if (current_fps == 0) Time::Instance()->SetFPS(60);
					if (current_fps == 1) Time::Instance()->SetFPS(48);
					if (current_fps == 2) Time::Instance()->SetFPS(24);
				};

				ImGui::Separator();
				ImGui::Checkbox("Show CheckerBoard BG", &m_ShowCheckerboardBg);
			}

			ImGui::Separator();
			if (ImGui::CollapsingHeader("Physics##Scene Config", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Checkbox("Show Collider Box outlines", &m_ShowColliderOutline))
				{
					Physics::ShowColliderOutline(m_ShowColliderOutline);
				}

				if (ImGui::Checkbox("Show Collision Contact points", &m_EnableCollisionContactPoints))
				{
					Physics::ShowCollisionsContactPoints(m_EnableCollisionContactPoints);
				}
			}

			ImGui::Separator();
			if (ImGui::CollapsingHeader("Scene debug##Scene Config", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::Checkbox("Enable Scene debug mode", &m_EnableSceneDebugMode))
				{
					Scene::EnableDebugMode(m_EnableSceneDebugMode);
				}
			}

			ImGui::End(); //Config window
		}
	}

	bool SceneConfig::ShowCheckerboardBg()
	{
		return m_ShowCheckerboardBg;
	}

}