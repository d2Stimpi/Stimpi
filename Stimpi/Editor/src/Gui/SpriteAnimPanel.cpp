#include "stpch.h"
#include "Gui/SpriteAnimPanel.h"

#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Core/Project.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	
	SpriteAnimPanel::SpriteAnimPanel()
	{
		// Nero sprite test
		AssetHandle handle = Project::GetEditorAssetManager()->RegisterAsset({ AssetType::TEXTURE, Project::GetAssestsDir() / "sprite_sheets\/nero_slap_anim-sheet.png" });
		m_SubTextureDisplay = std::make_shared<SubTexture>(handle, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 24, 24 });
		m_Sptire = std::make_shared<Sprite>(m_SubTextureDisplay.get(), 5, 1);
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
					m_Sptire->SetCurrentFrame(m_CurrentFrame);
				}
			}
			if (spriteState == SpriteState::RUNNING) ImGui::PopItemFlag();
			ImGui::PopItemWidth();

			glm::vec2 textureMin = m_SubTextureDisplay->GetUVMin();
			glm::vec2 textureMax = m_SubTextureDisplay->GetUVMax();
			ImVec2 uvMin = ImVec2{ textureMin.x, textureMax.y };
			ImVec2 uvMax = ImVec2{ textureMax.x, textureMin.y };

			if (m_SubTextureDisplay->GetTexture())
				ImGui::Image((ImTextureID)m_SubTextureDisplay->GetTextureID(), ImVec2{ 80 , 80 }, uvMin, uvMax);

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

			ImGui::PushItemWidth(80);
			if (ImGui::InputFloat("Duration##Sptire", &m_Duration))
			{
				m_Sptire->SetDuration(m_Duration);
			}
			ImGui::PopItemWidth();

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