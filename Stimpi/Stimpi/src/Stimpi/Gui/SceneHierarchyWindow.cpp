#include "stpch.h"
#include "Stimpi/Gui/SceneHierarchyWindow.h"

#include "Stimpi/Gui/Components/UIPayload.h"
#include "Stimpi/Gui/EditorUtils.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"

#include "Stimpi/Utils/PlatformUtils.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

#include <filesystem>

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
				// Add Entity pop-up
				if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("scene_hierarchy");
				}

				if (ImGui::BeginPopup("scene_hierarchy"))
				{
					if (ImGui::Selectable("Add Entity"))
					{
						s_SelectedEntity = m_ActiveScene->CreateEntity("NewEntity");
					}
					ImGui::EndPopup();
				}

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
						if (s_SelectedEntity)
						{
							m_ActiveScene->RemoveEntity(s_SelectedEntity);
							s_SelectedEntity = {};
						}
					}

					for (auto entity : m_ActiveScene->m_Entities)
					{
						ImGui::PushID((uint32_t)entity);
						auto entityTag = entity.GetComponent<TagComponent>().m_Tag;
						
						if (s_SelectedEntity == entity)
						{
							EditorUtils::RenderSelection();
						}

						if (ImGui::TreeNodeEx((void*)&entity, leaf_flags,"%s", entityTag.c_str()))
						{
							if (ImGui::IsItemClicked())
							{
								s_SelectedEntity = entity;
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

	void SceneHierarchyWindow::SetPickedEntity(Entity picked)
	{
		if (picked)
			s_SelectedEntity = picked;
	}

	Stimpi::Entity SceneHierarchyWindow::GetSelectedEntity()
	{
		return s_SelectedEntity;
	}

	void SceneHierarchyWindow::ComponentInspectorWidget()
	{
		if (ImGui::Begin("Component inspector", &m_ShowInspect))
		{
			ShowSelectedEntityComponents((bool)s_SelectedEntity);
			m_InspectFunc();
		}
		ImGui::End(); // ComponentInspectorWidget
	}

	void SceneHierarchyWindow::TagComponentLayout(TagComponent& component)
	{
		char tagInputBuff[32] = { "EntityTag" };

		if (component.m_Tag.length() < 32)
		{
			strcpy_s(tagInputBuff, component.m_Tag.c_str());
		}

		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 105);
		if (ImGui::InputText("##TagComponent", tagInputBuff, sizeof(tagInputBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			component.m_Tag = std::string(tagInputBuff);
		}
		EditorUtils::SetActiveItemCaptureKeyboard(false);
		ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 90); AddComponentLayout();
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
		std::filesystem::path texturePath = component.m_FilePath.c_str();
		if (ImGui::Button("Texture##TextureComponent"))
		{
			std::string filePath = FileDialogs::OpenFile("Texture (*.jpg)\0*.jpg\0(*.png)\0*.png\0");
			if (!filePath.empty())
			{
				component.SetTexture(filePath);
			}
		}

		UIPayload::BeginTarget(PAYLOAD_TEXTURE, [&component](void* data, uint32_t size) {
			std::string strData = std::string((char*)data, size);
			ST_CORE_INFO("Texture data dropped: {0}", strData.c_str());
			component.SetPayload(strData);
			});

		ImGui::SameLine();
		ImGui::Text("%s", texturePath.filename().string().c_str());

		
	}

	void SceneHierarchyWindow::CameraComponentLayout(CameraComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		glm::vec4 view = component.m_Camera->GetOrthoCamera()->GetViewQuad();

		ImGui::Text("CameraComponent"); ImGui::SameLine();
		if (ImGui::Button("Remove##Camera"))
		{
			s_SelectedEntity.RemoveComponent<CameraComponent>();
		}
		ImGui::Checkbox("Main##Camera", &component.m_IsMain);

		ImGui::PushItemWidth(80.0f);
		ImGui::Text("left:"); 
		ImGui::SameLine(60.f);
		if (ImGui::InputFloat("##CameraComponent left input", &view.x, NULL, NULL, "%.3f", flags))
			component.m_Camera->SetOrthoView(view);

		ImGui::Text("right:");
		ImGui::SameLine(60.f);
		if (ImGui::InputFloat("##CameraComponent right input", &view.y, NULL, NULL, "%.3f", flags))
			component.m_Camera->SetOrthoView(view);

		ImGui::Text("bottom:"); 
		ImGui::SameLine(60.f);
		if (ImGui::InputFloat("##CameraComponent bottom input", &view.z, NULL, NULL, "%.3f", flags))
			component.m_Camera->SetOrthoView(view);

		ImGui::Text("top:"); 
		ImGui::SameLine(60.f);
		if (ImGui::InputFloat("##CameraComponent top input", &view.w, NULL, NULL, "%.3f", flags))
			component.m_Camera->SetOrthoView(view);

		float camZoom = 1.f / component.m_Camera->GetZoomFactor();
		ImGui::Text("zoom:");
		ImGui::SameLine(60.f);
		if (ImGui::InputFloat("##CameraComponent zoom input", &camZoom, NULL, NULL, "%.3f", flags))
			component.m_Camera->SetZoomFactor(1.f / camZoom);

		ImGui::PopItemWidth();
	}

	void SceneHierarchyWindow::AddComponentLayout()
	{
		ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton;
		const char* selectedPreview = "Add Component";

		ImGui::PushItemWidth(100.0f);
		if (ImGui::BeginCombo("##AddComponentWidget", selectedPreview, flags))
		{
			if (!s_SelectedEntity.HasComponent<QuadComponent>())
			{
				if (ImGui::Selectable("QuadComponent##AddComponent"))
				{
					s_SelectedEntity.AddComponent<QuadComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<TextureComponent>())
			{
				if (ImGui::Selectable("TextureComponent##AddComponent"))
				{
					s_SelectedEntity.AddComponent<TextureComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::Selectable("CameraComponent##AddComponent"))
				{
					s_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
	}

	void SceneHierarchyWindow::ShowSelectedEntityComponents(bool show)
	{
		if (show)
		{
			m_InspectFunc = [this]()
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
			};
		}
		else
		{
			m_InspectFunc = []() {
				ImGui::Text("Nothing selected");
			};
		}
	}

}