#include "Stimpi/Gui/SceneConfigWindow.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	Entity s_SelectedEntity;

	SceneConfigWindow::SceneConfigWindow()
	{
		m_InspectFunc = []() {
			ImGui::Text("Nothing selected");
		};
	}

	SceneConfigWindow::~SceneConfigWindow()
	{

	}

	void SceneConfigWindow::Draw()
	{
		if (m_Show)
		{
			auto currentScene = SceneManager::Instance()->GetActiveScene();
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			// ImGuiTreeNodeFlags_NoTreePushOnOpen - does not require ImGui::TreePop() call
			ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; 

			ImGui::Begin("Scene Hierarchy", &m_Show);

			if (currentScene)
			{
				if (ImGui::TreeNodeEx((void*)&currentScene, node_flags | ImGuiTreeNodeFlags_DefaultOpen, "Scene"))
				{
					for (auto entity : currentScene->m_Entities)
					{
						ImGui::PushID((uint32_t)entity);
						auto entityTag = entity.GetComponent<TagComponent>().m_Tag;

						/* was node_flag */
						if (ImGui::TreeNodeEx((void*)&entity, leaf_flags,"%s", entityTag.c_str()))
						{
							if (ImGui::IsItemClicked())
							{
								s_SelectedEntity = entity;
								m_InspectFunc = [this, entityTag]()
								{
									ImGui::Text("Entity");
									ImGui::Text("Name: %s", entityTag.c_str());

									if (s_SelectedEntity.HasComponent<QuadComponent>())
									{
										ImGui::Separator();
										auto& component = s_SelectedEntity.GetComponent<QuadComponent>();
										QuadComponentLayout(component);
									}

									if (s_SelectedEntity.HasComponent<TextureComponent>())
									{
										ImGui::Separator();
										auto& component = s_SelectedEntity.GetComponent<TextureComponent>();
										TextureComponentLayout(component);
									}

									AddComponentLayout();
								};
							}

							//Show components as sub elements
							/*if (entity.HasComponent<QuadComponent>())
							{
								auto& component = entity.GetComponent<QuadComponent>();
								static bool selected = false;
								
								//ImGui::SetNextItemAllowOverlap();
								//ImGui::Selectable("##quad_node", &selected); ImGui::SameLine();

								if (ImGui::TreeNodeEx((void*)&component, leaf_flags, "Quad"))
								{
									QuadComponentClick(component, entityTag);
								}
							}

							if (entity.HasComponent<TextureComponent>())
							{
								auto& component = entity.GetComponent<TextureComponent>();
								if (ImGui::TreeNodeEx((void*)&component, leaf_flags, "Texture"))
								{
									TextureComponentClick(component, entityTag);
								}
							}*/
							//ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
			}

			// Sub-window: Showing stats for each component
			ComponentInspectorWidget();

			ImGui::End(); //Config window
		}
	}

	void SceneConfigWindow::ComponentInspectorWidget()
	{
		if (ImGui::Begin("Component inspector", &m_ShowInspect))
		{
			m_InspectFunc();
		}
		ImGui::End(); // ComponentInspectorWidget
	}

	void SceneConfigWindow::QuadComponentLayout(QuadComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		float x = component.m_X;
		float y = component.m_Y;
		float w = component.m_Width;
		float h = component.m_Height;

		ImGui::Text("QuadComponent"); ImGui::SameLine();
		if (ImGui::Button("Remove##Quad"))
		{
			s_SelectedEntity.RemoveComponent<QuadComponent>();
		}
		ImGui::PushItemWidth(80.0f);
		ImGui::Text("X:"); ImGui::SameLine();
		if (ImGui::InputFloat("##QuadComponent X input", &x, NULL, NULL, "%.3f", flags))
			component.m_X = x;
		ImGui::Text("Y:"); ImGui::SameLine();
		if (ImGui::InputFloat("##QuadComponent Y input", &y, NULL, NULL, "%.3f", flags))
			component.m_Y = y;
		ImGui::Text("W:"); ImGui::SameLine();
		if (ImGui::InputFloat("##QuadComponent W input", &w, NULL, NULL, "%.3f", flags))
			component.m_Width = w;
		ImGui::Text("H:"); ImGui::SameLine();
		if (ImGui::InputFloat("##QuadComponent h input", &h, NULL, NULL, "%.3f", flags))
			component.m_Height = h;
		ImGui::PopItemWidth();
	}

	void SceneConfigWindow::TextureComponentLayout(TextureComponent& component)
	{
		ImGui::Text("TextureComponent"); ImGui::SameLine();
		if (ImGui::Button("Remove##Texture"))
		{
			s_SelectedEntity.RemoveComponent<TextureComponent>();
		}
		ImGui::Text("File path: %s", component.m_FilePath.c_str());
	}

	void SceneConfigWindow::AddComponentLayout()
	{
		ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;
		const char* items[] = { "QuadComponent", "TextureComponent" };
		static uint32_t selection = 0;
		const char* selectedPreview = items[selection];

		ImGui::Separator();
		ImGui::PushItemWidth(160.0f);
		if (ImGui::BeginCombo("##AddComponentWidget", selectedPreview, flags))
		{
			for (uint32_t i = 0; i < IM_ARRAYSIZE(items); i++)
			{
				const bool isSelected = (selection == i);
				if (ImGui::Selectable(items[i], isSelected))
					selection = i;

				// Initial focus
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		// Add component button
		if (ImGui::Button("Add Component"))
		{
			ST_CORE_INFO("AddComponent button clicked - {0}", items[selection]);
			switch (selection)
			{
			case 0: //QuadComponent
				if (!s_SelectedEntity.HasComponent<QuadComponent>())
				{
					s_SelectedEntity.AddComponent<QuadComponent>();
				}
				break;
			case 1: //TextureComponent
				if (!s_SelectedEntity.HasComponent<TextureComponent>())
				{
					s_SelectedEntity.AddComponent<TextureComponent>("Capture.jpg");
				}
				break;
			default:
				ST_CORE_ERROR("AddComponentLayout - shoul't enter here");
				break;
			}
		}
	}
}