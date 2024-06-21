#include "stpch.h"
#include "Gui/Stats/EditorConfig.h"

#include "Stimpi/Core/Time.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/Assets/AssetManager.h"
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
					ImGui::Text("Application FPS %.1f", Time::Instance()->GetActiveFPS());
					ImGui::Text("Average %.1f ms/frame", 1000.0f / Time::Instance()->GetActiveFPS());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Asset Manager##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Assets usage count: %d", AssetManager::GetLoadedAssetsCount());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Renderer 2D##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("Frame draw calls: %d", Renderer2D::Instance()->GetFrameDrawCallCount());
					ImGui::Text("Frame render cmd count: %d", Renderer2D::Instance()->GetFrameRednerCmdCount());
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Mono Runtime##Global Stats", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Text("GC used size %ld", (long)ScriptEngine::GetGCUsedSize());
					ImGui::Text("GC heap size %ld", (long)ScriptEngine::GetGCHeapSize());
					/*if (ImGui::Button("GC Collect"))
					{
						ScriptEngine::ForceGCCollect();
					}*/
					if (ImGui::Button("Reload Assembly"))
					{
						ScriptEngine::ReloadAssembly();
					}
				}
			}
			ImGui::End();
		}
	}

}