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
										QuadComponentLayout(component, entityTag);
									}

									if (s_SelectedEntity.HasComponent<TextureComponent>())
									{
										ImGui::Separator();
										auto& component = s_SelectedEntity.GetComponent<TextureComponent>();
										TextureComponentLayout(component, entityTag);
									}
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

	void SceneConfigWindow::QuadComponentLayout(QuadComponent& component, const std::string& name)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		float x = component.m_X;
		float y = component.m_Y;
		float w = component.m_Width;
		float h = component.m_Height;

		ImGui::Text("QuadComponent");
		ImGui::Text("Tag: %s", name.c_str());
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

	void SceneConfigWindow::TextureComponentLayout(TextureComponent& component, const std::string& name)
	{
		ImGui::Text("TextureComponent");
		ImGui::Text("Tag: %s", name.c_str());
		ImGui::Text("File path: %s", component.m_FilePath.c_str());
	}

}