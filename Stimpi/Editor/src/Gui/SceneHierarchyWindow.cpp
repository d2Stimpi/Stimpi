#include "stpch.h"
#include "Gui/SceneHierarchyWindow.h"

#include "Gui/Components/UIPayload.h"
#include "Gui/EditorUtils.h"

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

		ImGui::Separator();
		if (ImGui::CollapsingHeader("TagComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
			if (ImGui::InputText("##TagComponent", tagInputBuff, sizeof(tagInputBuff), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				component.m_Tag = std::string(tagInputBuff);
			}
			EditorUtils::SetActiveItemCaptureKeyboard(false);
			ImGui::PopItemWidth();
		}
	}

	void SceneHierarchyWindow::QuadComponentLayout(QuadComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		float x = component.m_X;
		float y = component.m_Y;
		float w = component.m_Width;
		float h = component.m_Height;
		float rota = component.m_Rotation;

		ImGui::Separator();
		if (ImGui::CollapsingHeader("QuadComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::PushItemWidth(80.0f);
			if (ImGui::InputFloat("##QuadComponent X input", &x, NULL, NULL, "%.3f", flags))
				component.m_X = x;
			ImGui::SameLine(); ImGui::Text("X position");
			if (ImGui::InputFloat("##QuadComponent Y input", &y, NULL, NULL, "%.3f", flags))
				component.m_Y = y;
			ImGui::SameLine(); ImGui::Text("Y position");
			if (ImGui::InputFloat("##QuadComponent W input", &w, NULL, NULL, "%.3f", flags))
				component.m_Width = w;
			ImGui::SameLine(); ImGui::Text("Width");
			if (ImGui::InputFloat("##QuadComponent H input", &h, NULL, NULL, "%.3f", flags))
				component.m_Height = h;
			ImGui::SameLine(); ImGui::Text("Height");
			if (ImGui::InputFloat("##QuadComponent R input", &rota, NULL, NULL, "%.3f", flags))
				component.m_Rotation = rota;
			ImGui::SameLine(); ImGui::Text("Rotation");
			ImGui::PopItemWidth();
			if (ImGui::Button("Remove##Quad"))
			{
				s_SelectedEntity.RemoveComponent<QuadComponent>();
			}
		}
	}

	void SceneHierarchyWindow::TextureComponentLayout(TextureComponent& component)
	{
		ImGui::Separator();
		if (ImGui::CollapsingHeader("TextureComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::filesystem::path texturePath = component.m_FilePath.c_str();

			ImGui::PushItemWidth(80.0f);
			if (ImGui::Button("Texture##TextureComponent"))
			{
				std::string filePath = FileDialogs::OpenFile("Texture (*.jpg)\0*.jpg\0(*.png)\0*.png\0");
				if (!filePath.empty())
				{
					component.SetTexture(filePath);
				}
			}
			ImGui::PopItemWidth();


			UIPayload::BeginTarget(PAYLOAD_TEXTURE, [&component](void* data, uint32_t size) {
				std::string strData = std::string((char*)data, size);
				ST_CORE_INFO("Texture data dropped: {0}", strData.c_str());
				component.SetPayload(strData);
				});

			ImGui::SameLine();
			if (texturePath.has_filename())
				ImGui::Text("%s", texturePath.filename().string().c_str());
			else
				ImGui::Text("Add Texture");

			if (ImGui::Button("Remove##Texture"))
			{
				s_SelectedEntity.RemoveComponent<TextureComponent>();
			}
		}
	}

	void SceneHierarchyWindow::CameraComponentLayout(CameraComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		glm::vec4 view = component.m_Camera->GetOrthoCamera()->GetViewQuad();

		ImGui::Separator();
		if (ImGui::CollapsingHeader("CameraComponent", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Main##Camera", &component.m_IsMain);

			ImGui::PushItemWidth(80.0f);
			if (ImGui::InputFloat("##CameraComponent left input", &view.x, NULL, NULL, "%.3f", flags))
				component.m_Camera->SetOrthoView(view);
			ImGui::SameLine();
			ImGui::Text("Left");

			if (ImGui::InputFloat("##CameraComponent right input", &view.y, NULL, NULL, "%.3f", flags))
				component.m_Camera->SetOrthoView(view);
			ImGui::SameLine();
			ImGui::Text("Right");

			if (ImGui::InputFloat("##CameraComponent bottom input", &view.z, NULL, NULL, "%.3f", flags))
				component.m_Camera->SetOrthoView(view);
			ImGui::SameLine();
			ImGui::Text("Bottom");

			if (ImGui::InputFloat("##CameraComponent top input", &view.w, NULL, NULL, "%.3f", flags))
				component.m_Camera->SetOrthoView(view);
			ImGui::SameLine();
			ImGui::Text("Top");

			float camZoom = 1.f / component.m_Camera->GetZoomFactor();
			if (ImGui::InputFloat("##CameraComponent zoom input", &camZoom, NULL, NULL, "%.3f", flags))
				component.m_Camera->SetZoomFactor(1.f / camZoom);
			ImGui::SameLine();
			ImGui::Text("Camera Zoom");
			ImGui::PopItemWidth();

			if (ImGui::Button("Remove##Camera"))
			{
				s_SelectedEntity.RemoveComponent<CameraComponent>();
			}
		}
	}

	void SceneHierarchyWindow::AddComponentLayout()
	{
		ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton;
		const char* selectedPreview = "Add Component";

		ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth());
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
			AddComponentLayout();

			if (s_SelectedEntity.HasComponent<TagComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<TagComponent>();
				TagComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<QuadComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<QuadComponent>();
				QuadComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<TextureComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<TextureComponent>();
				TextureComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<CameraComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<CameraComponent>();
				CameraComponentLayout(component);
			}
		}
		else
		{
			ImGui::Text("Nothing selected");
		}
	}

}