#include "stpch.h"
#include "Gui/Stats/EditorConfig.h"

#include "Stimpi/Core/Time.h"
#include "Stimpi/Debug/Statistics.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/Assets/AssetManagerB.h"
#include "Stimpi/Scripting/ScriptEngine.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	
	EditorConfig::EditorConfig()
	{

	}

	EditorConfig::~EditorConfig()
	{

	}

	void EditorConfig::OnImGuiRender()
	{
		if (m_Show)
		{
			if (ImGui::Begin("Global Stats", &m_Show))
			{
				if (ImGui::CollapsingHeader("FPS##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					FrameTimeSplit& frameTimeSplit = Statistics::GetFrameTimeSplitData();

					ImGui::Text("Application FPS %.1f", Time::Instance()->GetActiveFPS());
					ImGui::Text("Average %.1f ms/frame", 1000.0f / Time::Instance()->GetActiveFPS());
					ImGui::Separator();
					ImGui::Text("Idling    %.3f ms, frame: %.3f", (float)frameTimeSplit.m_IdleTimeAvg / 1000.0f, (float)frameTimeSplit.m_IdleTime / 1000.0f);
					ImGui::Text("Rendering %.3f ms, frame: %.3f", (float)frameTimeSplit.m_RenderingTimeAvg / 1000.0f, (float)frameTimeSplit.m_RenderingTime / 1000.0f);
					ImGui::Text("Physics   %.3f ms, frame: %.3f", (float)frameTimeSplit.m_PhysicsSimTimeAvg / 1000.0f, (float)frameTimeSplit.m_PhysicsSimTime / 1000.0f);
					ImGui::Text("Scripting %.3f ms, frame: %.3f", (float)frameTimeSplit.m_ScriptingTimeAvg / 1000.0f, (float)frameTimeSplit.m_ScriptingTime / 1000.0f);
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Asset Manager##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Assets usage count: %d", AssetManagerB::GetLoadedAssetsCount());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Renderer 2D##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Frame draw calls: %d", Renderer2D::Instance()->GetFrameDrawCallCount());
					ImGui::Text("Frame render cmd count: %d", Renderer2D::Instance()->GetFrameRednerCmdCount());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Physics##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Active collisions %d", Physics::GetActiveCollisionsCount());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Mono Runtime##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("GC used size %ld", (long)ScriptEngine::GetGCUsedSize());
					ImGui::Text("GC heap size %ld", (long)ScriptEngine::GetGCHeapSize());
					ImGui::Text("GC handles %d", ScriptEngine::GetGCHandleCount());
					// TODO: fix these actions - atm disabled
					//ImGui::BeginDisabled();
					if (ImGui::Button("GC Collect"))
					{
						ScriptEngine::ForceGCCollect();
					}
					if (ImGui::Button("Reload Assembly"))
					{
						ScriptEngine::ReloadAssembly();
					}
					//ImGui::EndDisabled();
				}
			}
			ImGui::End();
		}
	}

}