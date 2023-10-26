#include "stpch.h"
#include "Stimpi/Gui/SceneHierarchyWindow.h"

#include "Stimpi/Gui/Components/UIPayload.h"
#include "Stimpi/Gui/EditorUtils.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	Entity s_SelectedEntity = {};

	SceneHierarchyWindow::SceneHierarchyWindow()
	{
		m_InspectFunc = []() {
			ImGui::Text("Nothing selected");
		};

		OnSceneChangedListener onScneeChanged = [&]() {
			ST_CORE_INFO("Scene change detected!");
			m_ActiveScene = SceneManager::Instance()->GetActiveScene();
			s_SelectedEntity = {};
			m_InspectFunc = []() {
				ImGui::Text("Nothing selected");
			};
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);

		m_ActiveScene = SceneManager::Instance()->GetActiveScene();
	}

	SceneHierarchyWindow::~SceneHierarchyWindow()
	{

	}

	void SceneHierarchyWindow::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
			// ImGuiTreeNodeFlags_NoTreePushOnOpen - does not require ImGui::TreePop() call
			ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; 

			ImGui::Begin("Scene Hierarchy", &m_Show);

			if (m_ActiveScene)
			{
				if (ImGui::TreeNodeEx((void*)&m_ActiveScene, node_flags | ImGuiTreeNodeFlags_DefaultOpen, "Scene"))
				{
					// Add Entity Button
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 60);
					if (ImGui::Button(" + ##NewEntity"))
					{
						s_SelectedEntity = m_ActiveScene->CreateEntity("NewEntity");
					}

					// Remove Entity Button
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 30);
					if (ImGui::Button(" - ##RemoveEntity"))
					{
						// TODO: When GUID is added - check if selected Entity is valid
						if (s_SelectedEntity)
						{
							m_ActiveScene->RemoveEntity(s_SelectedEntity);
							m_InspectFunc = []() {
								ImGui::Text("Nothing selected");
							};
							s_SelectedEntity = {};
						}
					}

					for (auto entity : m_ActiveScene->m_Entities)
					{
						ImGui::PushID((uint32_t)entity);
						auto entityTag = entity.GetComponent<TagComponent>().m_Tag;
						
						// Selection - TODO: will handle itself when GUID is added to Entity (currently shows 0 index selection when it should not) 
						if (s_SelectedEntity == entity)
						{
							EditorUtils::RenderSelection();
						}

						if (ImGui::TreeNodeEx((void*)&entity, leaf_flags,"%s", entityTag.c_str()))
						{
							if (ImGui::IsItemClicked())
							{
								s_SelectedEntity = entity;
								m_InspectFunc = [this, entityTag]()
								{
									if (s_SelectedEntity.HasComponent<TagComponent>())
									{
										auto& component = s_SelectedEntity.GetComponent<TagComponent>();
										TagComponentLayout(component);
									}

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

									if (s_SelectedEntity.HasComponent<CameraComponent>())
									{
										ImGui::Separator();
										auto& component = s_SelectedEntity.GetComponent<CameraComponent>();
										CameraComponentLayout(component);
									}

									AddComponentLayout();
								};
							}
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

	void SceneHierarchyWindow::ComponentInspectorWidget()
	{
		if (ImGui::Begin("Component inspector", &m_ShowInspect))
		{
			m_InspectFunc();
		}
		ImGui::End(); // ComponentInspectorWidget
	}

	void SceneHierarchyWindow::TagComponentLayout(TagComponent& component)
	{
		char tagInputBuff[12] = { "EntityTag" };

		if (component.m_Tag.length() < 12)
		{
			strcpy_s(tagInputBuff, component.m_Tag.c_str());
		}

		if (ImGui::InputText("##TagComponent", tagInputBuff, sizeof(tagInputBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			component.m_Tag = std::string(tagInputBuff);
		}
		EditorUtils::SetActiveItemCaptureKeyboard(false);
		ImGui::SameLine(); ImGui::Text("Tag");
	}

	void SceneHierarchyWindow::QuadComponentLayout(QuadComponent& component)
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

	void SceneHierarchyWindow::TextureComponentLayout(TextureComponent& component)
	{
		ImGui::Text("TextureComponent"); ImGui::SameLine();
		if (ImGui::Button("Remove##Texture"))
		{
			s_SelectedEntity.RemoveComponent<TextureComponent>();
		}
		ImGui::Text("File path: %s", component.m_FilePath.c_str());


		UIPayload::BeginTarget(PAYLOAD_TEXTURE, [&component](void* data, uint32_t size) {
				std::string strData = std::string((char*)data, size);
				ST_CORE_INFO("Texture data dropped: {0}", strData.c_str());
				component.SetPayload(strData);	// TODO: investigate app stuck
			});
	}

	void SceneHierarchyWindow::CameraComponentLayout(CameraComponent& component)
	{
		ImGui::Text("CameraComponent"); ImGui::SameLine();
		if (ImGui::Button("Remove##Camera"))
		{
			s_SelectedEntity.RemoveComponent<CameraComponent>();
		}
		if (ImGui::Checkbox("Main##Camera", &component.m_IsMain));
	}

	void SceneHierarchyWindow::AddComponentLayout()
	{
		ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft;
		const char* items[] = { "QuadComponent", "TextureComponent", "CameraComponent" };
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
			case 2: //CameraComponent
				if (!s_SelectedEntity.HasComponent<CameraComponent>())
				{
					s_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}
				break;
			default:
				ST_CORE_ERROR("AddComponentLayout - shoul't enter here");
				break;
			}
		}
	}
}