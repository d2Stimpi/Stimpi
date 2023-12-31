#include "stpch.h"
#include "Gui/SceneHierarchyWindow.h"

#include "Gui/Components/UIPayload.h"
#include "Gui/Components/SearchPopup.h"
#include "Gui/EditorUtils.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"

#include "Stimpi/Utils/PlatformUtils.h"

#include "Stimpi/Scripting/ScriptEngine.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

#include <glm/gtc/type_ptr.hpp>
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
				CreateEntityPopup();

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
		ImGui::End();
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
		ImGui::PopItemWidth();
	}

	void SceneHierarchyWindow::QuadComponentLayout(QuadComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Quad##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat2("Position##Quad", glm::value_ptr(component.m_Position));
			ImGui::DragFloat2("Size##Quad", glm::value_ptr(component.m_Size));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Rotation", &component.m_Rotation, 0.01);
			ImGui::PopItemWidth();
			ImGui::Separator();
			if (ImGui::Button("Remove##Quad"))
			{
				s_SelectedEntity.RemoveComponent<QuadComponent>();
			}
		}
	}

	void SceneHierarchyWindow::CircleComponentLayout(CircleComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Circle##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat2("Position##Circle", glm::value_ptr(component.m_Position));
			ImGui::DragFloat2("Size##Circle", glm::value_ptr(component.m_Size));
			ImGui::ColorEdit4("Color##CircleColor", glm::value_ptr(component.m_Color));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Thickness", &component.m_Thickness, 0.01);
			ImGui::DragFloat("Fade", &component.m_Fade, 0.001);
			ImGui::PopItemWidth();
			ImGui::Separator();
			if (ImGui::Button("Remove##Circle"))
			{
				s_SelectedEntity.RemoveComponent<CircleComponent>();
			}
		}
	}

	void SceneHierarchyWindow::ScriptComponentLayout(ScriptComponent& component)
	{
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Script##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static char scriptName[64];
			strcpy(scriptName, component.m_ScriptName.c_str());

			bool scriptClassExists = ScriptEngine::HasScriptClass(component.m_ScriptName);
			static bool showPopup = false;
			ImGui::InputText("##ScriptComponentPreview", scriptName, sizeof(scriptName), ImGuiInputTextFlags_ReadOnly);
			if (ImGui::IsItemClicked())
			{
				ImGui::SetNextWindowPos(ImGui::GetMousePos());
				ImGui::OpenPopup("ScriptSelectPoput");
				showPopup = true;
			}
			
			if (showPopup)
			{
				auto filterData = ScriptEngine::GetScriptClassNames();
				if (SearchPopup::OnImGuiRender("ScriptSelectPoput", &showPopup, filterData))
				{
					component.m_ScriptName = SearchPopup::GetSelection();
					ScriptEngine::OnScriptComponentAdd(component.m_ScriptName, s_SelectedEntity);
				}
			}

			// Fields & Properties
			auto scriptClass = ScriptEngine::GetScriptClassByName(component.m_ScriptName);
			if (scriptClass != nullptr)
			{
				ImGuiInputTextFlags fieldInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

				ImGui::Separator();
				auto fields = scriptClass->GetAllFields();
				auto tagName = s_SelectedEntity.GetComponent<TagComponent>().m_Tag;
				for (auto item : fields)
				{
					std::string fieldName = ScriptEngine::GetFieldName(item).c_str();

					// Populate with data from Entity's class Instance
					std::shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(s_SelectedEntity);
					if (scriptInstance != nullptr)
					{
						float fNum = 0.0f;
						auto field = scriptInstance->GetScriptFieldFromMonoField(item);
						field->ReadFieldValue(&fNum);
						if (ImGui::InputFloat(fmt::format("{}##{}", fieldName, tagName).c_str(), &fNum, 0.0f, 0.0f, "%.3f", fieldInputFlags))
						{
							field->SetFieldValue(&fNum);
						}
					}
				}
			}

			ImGui::Separator();
			if (ImGui::Button("Remove##Script"))
			{
				ScriptEngine::OnScriptComponentRemove(s_SelectedEntity);
				s_SelectedEntity.RemoveComponent<ScriptComponent>();
			}
		}
	}

	void SceneHierarchyWindow::SpriteComponentLayout(SpriteComponent& component)
	{
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Sprite##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			std::filesystem::path texturePath = component.m_FilePath.c_str();

			ImGui::ColorEdit4("Color##SpriteColor", glm::value_ptr(component.m_Color));

			ImGui::PushItemWidth(80.0f);
			if (ImGui::Button("Texture##SpriteComponent"))
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

			//ImGui::SameLine();
			ImGui::Checkbox("Enable Texture##Texture_SpriteComponent", &component.m_Enable);

			ImGui::Separator();
			if (ImGui::Button("Remove##Texture"))
			{
				s_SelectedEntity.RemoveComponent<SpriteComponent>();
			}
		}
	}

	void SceneHierarchyWindow::CameraComponentLayout(CameraComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		glm::vec4 view = component.m_Camera->GetOrthoCamera()->GetViewQuad();

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Camera##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Main##Camera", &component.m_IsMain);

			ImGui::Text("Camera Viewport:");
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("X##CameraComponent left input", &view.x);

			ImGui::SameLine();
			ImGui::DragFloat("Y##CameraComponent bottom input", &view.z);

			ImGui::DragFloat("W##CameraComponent right input", &view.y);

			ImGui::SameLine();
			ImGui::DragFloat("H##CameraComponent top input", &view.w);

			component.m_Camera->SetOrthoView(view);

			ImGui::Separator();
			if (ImGui::Button("Remove##Camera"))
			{
				s_SelectedEntity.RemoveComponent<CameraComponent>();
			}
		}
	}

	void SceneHierarchyWindow::RigidBody2DComponentLayout(RigidBody2DComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Rigid Body 2D##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.m_Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i <= 2; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.m_Type = (RigidBody2DComponent::BodyType)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.m_FixedRotation);

			ImGui::Separator();
			if (ImGui::Button("Remove##RigidBody2D"))
			{
				s_SelectedEntity.RemoveComponent<RigidBody2DComponent>();
			}
		}
	}

	void SceneHierarchyWindow::BoxCollider2DComponentLayout(BoxCollider2DComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Box Collider##ComponentName", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.m_Offset));
			ImGui::DragFloat2("Size##Collider", glm::value_ptr(component.m_Size));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Density", &component.m_Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.m_Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.m_Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.m_RestitutionThreshold, 0.01f, 0.0f);
			ImGui::PopItemWidth();

			ImGui::Separator();
			if (ImGui::Button("Remove##BoxCollider"))
			{
				s_SelectedEntity.RemoveComponent<BoxCollider2DComponent>();
			}
		}
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
				if (ImGui::Selectable("Quad##AddComponent"))
				{
					s_SelectedEntity.AddComponent<QuadComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<CircleComponent>())
			{
				if (ImGui::Selectable("Circle##AddComponent"))
				{
					s_SelectedEntity.AddComponent<CircleComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<SpriteComponent>())
			{
				if (ImGui::Selectable("Sprite##AddComponent"))
				{
					s_SelectedEntity.AddComponent<SpriteComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<ScriptComponent>())
			{
				if (ImGui::Selectable("Script##AddComponent"))
				{
					s_SelectedEntity.AddComponent<ScriptComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::Selectable("Camera##AddComponent"))
				{
					s_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}
			}

			if (!s_SelectedEntity.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::Selectable("RigidBody2D##AddComponent"))
				{
					s_SelectedEntity.AddComponent<RigidBody2DComponent>();
				}
			}

			if (!s_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::Selectable("BoxCollider2D##AddComponent"))
				{
					s_SelectedEntity.AddComponent<BoxCollider2DComponent>();
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

			if (s_SelectedEntity.HasComponent<CircleComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<CircleComponent>();
				CircleComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<SpriteComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<SpriteComponent>();
				SpriteComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<ScriptComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<ScriptComponent>();
				ScriptComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<CameraComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<CameraComponent>();
				CameraComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<RigidBody2DComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<RigidBody2DComponent>();
				RigidBody2DComponentLayout(component);
			}

			if (s_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				auto& component = s_SelectedEntity.GetComponent<BoxCollider2DComponent>();
				BoxCollider2DComponentLayout(component);
			}
		}
		else
		{
			ImGui::Text("Nothing selected");
		}
	}

	void SceneHierarchyWindow::CreateEntityPopup()
	{
		if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("CreateEntityPopup");
		}

		if (ImGui::BeginPopup("CreateEntityPopup"))
		{
			if (ImGui::Selectable("Empty Entity"))
			{
				s_SelectedEntity = m_ActiveScene->CreateEntity("NewEntity");
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					s_SelectedEntity = m_ActiveScene->CreateEntity("Camera_Object");
					s_SelectedEntity.AddComponent<QuadComponent>();
					s_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Primitive"))
			{
				if (ImGui::MenuItem("Sprite"))
				{
					s_SelectedEntity = m_ActiveScene->CreateEntity("Sprite_Object");
					s_SelectedEntity.AddComponent<QuadComponent>(glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f), 0.0f);
					s_SelectedEntity.AddComponent<SpriteComponent>();
				}

				if (ImGui::MenuItem("Circle"))
				{
					s_SelectedEntity = m_ActiveScene->CreateEntity("Circle_Object");
					s_SelectedEntity.AddComponent<CircleComponent>(glm::vec2(0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}

}