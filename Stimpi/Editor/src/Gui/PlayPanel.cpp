#include "stpch.h"
#include "Gui/PlayPanel.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{

	PlayPanel::PlayPanel()
	{

	}

	PlayPanel::~PlayPanel()
	{

	}

	void PlayPanel::OnImGuiRender()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration;
		m_ActiveScene = SceneManager::Instance()->GetActiveScene();

		ImGui::Begin("PlayPanel", &m_Show, flags);

		ImGuiStyle style = ImGui::GetStyle();

		float size = ImGui::CalcTextSize("->  x").x + style.FramePadding.x * 2.0f;
		float region = ImGui::GetContentRegionAvail().x;
		float offset = (region - size) * 0.5;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		if (m_ActiveScene->GetRuntimeState() != RuntimeState::RUNNING)
		{
			if (ImGui::Button("->##PlayButton"))
			{
				ST_CORE_INFO("Play the scene.");
				m_ActiveScene->OnScenePlay();
				// Save scene as temp file so it can be restored on Scene Stop
				SceneManager::Instance()->SaveScene("tmp.d2s");
			}
		}
		else
		{
			if (ImGui::Button("=##PlayButton"))
			{
				ST_CORE_INFO("Pause the scene.");
				m_ActiveScene->OnScenePause();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("x##PlayButton"))
		{
			ST_CORE_INFO("Stop the scene.");
			m_ActiveScene->OnSceneStop();
			// Restore Scene form temp file
			SceneManager::Instance()->LoadScene("tmp.d2s");
		}

		ImGui::End();
	}

}