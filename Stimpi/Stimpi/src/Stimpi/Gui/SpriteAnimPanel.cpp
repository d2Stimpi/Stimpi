#include "stpch.h"
#include "Stimpi/Gui/SpriteAnimPanel.h"

#include "Stimpi/Scene/ResourceManager.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	
	SpriteAnimPanel::SpriteAnimPanel()
	{
		// Temp test
		auto texture = ResourceManager::Instance()->LoadTexture("..\/assets\/sprite_sheets\/sonic-sprite-sheet.png");
		m_SubTextureDisplay = std::make_shared<SubTexture>(texture, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ texture->GetWidth() / 10, texture->GetHeight() / 10 });
		m_Sptire = std::make_shared<Sprite>(m_SubTextureDisplay.get(), 10, 2.8);
		m_LoopAnim = m_Sptire->GetLooping();
		m_Duration = m_Sptire->GetDuration();
	}

	SpriteAnimPanel::~SpriteAnimPanel()
	{

	}

	void SpriteAnimPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			SpriteState spriteState = m_Sptire->GetSpriteState();
			float dragSpeed = (spriteState == SpriteState::RUNNING) ? 0.0f : 0.05f;
		
			ImGui::Begin("Sprite Animation", &m_Show);
			if (spriteState == SpriteState::RUNNING || spriteState == SpriteState::COMPELETED)
			{
				m_CurrentFrame = m_Sptire->GetCurrentFrame();
			}
			
			ImGui::PushItemWidth(40);
			if (spriteState == SpriteState::RUNNING) ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			if (ImGui::DragInt("Current Frame##Sprite", (int*)&m_CurrentFrame, 0.05f, 0, (int)m_Sptire->GetFramesCount() - 1))
			{
				if (spriteState != SpriteState::RUNNING)
				{
					ST_CORE_INFO("Selected frame: {0}", m_CurrentFrame);
					m_Sptire->SetCurrentFrame(m_CurrentFrame);
				}
			}
			if (spriteState == SpriteState::RUNNING) ImGui::PopItemFlag();
			ImGui::PopItemWidth();

			glm::vec2 textureMin = m_SubTextureDisplay->GetUVMin();
			glm::vec2 textureMax = m_SubTextureDisplay->GetUVMax();
			ImVec2 uvMin = ImVec2{ textureMin.x, textureMax.y };
			ImVec2 uvMax = ImVec2{ textureMax.x, textureMin.y };

			ImGui::Image((ImTextureID)m_SubTextureDisplay->GetTextureID(), ImVec2{ (float)m_SubTextureDisplay->GetSubWidht() , (float)m_SubTextureDisplay->GetSubHeight() }, uvMin, uvMax);

			ImGui::PushItemWidth(40);
			if (spriteState != SpriteState::RUNNING)
			{
				if (ImGui::Button(">##Sprite"))
				{
					m_Sptire->Start();
				}
			}
			else
			{
				if (ImGui::Button("=##Sprite"))
				{
					m_Sptire->Pause();
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("x##Sprite"))
			{
				m_Sptire->Stop();
				m_CurrentFrame = 0;
			}
			ImGui::PopItemWidth();

			if (ImGui::InputFloat("Duration##Sptire", &m_Duration))
			{
				m_Sptire->SetDuration(m_Duration);
				ST_CORE_INFO("Duration change: {0}", m_Duration);
			}

			if (ImGui::Checkbox("Loop##Sprite", &m_LoopAnim))
			{
				m_Sptire->SetLooping(m_LoopAnim);
			}

			ImGui::End();
		}
	}

	void SpriteAnimPanel::OnUpdate(Timestep ts)
	{
		m_Sptire->Update(ts);
	}

}