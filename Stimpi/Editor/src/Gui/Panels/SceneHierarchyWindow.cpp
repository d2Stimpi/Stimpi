#include "stpch.h"
#include "Gui/Panels/SceneHierarchyWindow.h"

#include "Gui/Components/UIPayload.h"
#include "Gui/Components/SearchPopup.h"
#include "Gui/EditorUtils.h"
#include "Gui/Components/ImGuiEx.h"
#include "Gui/Panels/ScriptFieldFragment.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/WindowManager.h"
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
	struct SceneHierarchyWindowContext
	{
		Entity m_SelectedEntity = {};
		bool m_WindowFocused = false;

		char m_SearchTextBuffer[64];

		SceneHierarchyWindowContext()
		{
			memset(m_SearchTextBuffer, 0, sizeof(m_SearchTextBuffer));
		}
	};

	SceneHierarchyWindowContext s_Context;

	SceneHierarchyWindow::SceneHierarchyWindow()
	{
		ScriptFieldFragment::RegisterScriptFieldFunctions();

		OnSceneChangedListener onScneeChanged = [&]() {
			ST_CORE_INFO("Scene change detected!");
			m_ActiveScene = SceneManager::Instance()->GetActiveScene();
			s_Context.m_SelectedEntity = {};
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);

		InputManager::Instance()->AddKeyboardEventHandler(new KeyboardEventHandler([&](KeyboardEvent event) -> bool 
			{
				if (EditorUtils::WantCaptureKeyboard() && s_Context.m_WindowFocused)
				{
					if (event.GetKeyCode() == ST_KEY_F && event.GetType() == KeyboardEventType::KEY_EVENT_DOWN)
					{
						if (m_ActiveScene != nullptr)
						{
							Camera* camera = m_ActiveScene->GetRenderCamera();
							Entity entity = s_Context.m_SelectedEntity;

							if (entity.HasComponent<QuadComponent>())
							{
								QuadComponent quad = entity.GetComponent<QuadComponent>();
								glm::vec2 view = { camera->GetViewportWidth(), camera->GetViewportHeight() };
								glm::vec3 camPos = camera->GetPosition();
								float zoom = camera->GetZoomFactor();
								camPos.x = quad.m_Position.x - view.x / 2.0f * zoom;
								camPos.y = quad.m_Position.y - view.y / 2.0f * zoom;
								camera->SetPosition(camPos);
							}
						}
						return true;
					}
				}
				return false;
			}));

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

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
			ImGui::Begin("Scene Hierarchy", &m_Show); 
			ImGui::PopStyleVar();

			s_Context.m_WindowFocused = ImGui::IsWindowFocused();

			if (m_ActiveScene)
			{
				// Add Entity button
				if (ImGuiEx::IconButton("##SceneHierarchyToolbarButtonPCH", EDITOR_ICON_CROSS))
				{
					//s_Context.m_SelectedEntity = m_ActiveScene->CreateEntity("NewEntity");
					ImGui::OpenPopup("CreateEntityPopup");
				}
				ImGui::SameLine(30.0f);
				ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - 46.0f);

				// Filter Entities
				ImGuiEx::SearchInput("##SceneHierarchySearchInput", s_Context.m_SearchTextBuffer, sizeof(s_Context.m_SearchTextBuffer), "All");

				// Remove Entity button
				ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 12);
				if (ImGui::Button(" - ##RemoveEntity"))
				{
					if (s_Context.m_SelectedEntity)
					{
						m_ActiveScene->RemoveEntity(s_Context.m_SelectedEntity);
						s_Context.m_SelectedEntity = {};
					}
				}
				ImGui::Separator();

				// Add Entity pop-up
				CreateEntityPopup();

				if (ImGui::TreeNodeEx((void*)&m_ActiveScene, node_flags | ImGuiTreeNodeFlags_DefaultOpen, "Scene"))
				{

					static Entity preSelect;
					for (auto entity : m_ActiveScene->m_Entities)
					{
						auto entityTag = entity.GetComponent<TagComponent>().m_Tag;

						std::string filterTagString = s_Context.m_SearchTextBuffer;
						if (filterTagString.empty() || entityTag.find(filterTagString) != std::string::npos)
						{
							ImGui::PushID((uint32_t)entity);

							if (s_Context.m_SelectedEntity == entity)
							{
								EditorUtils::RenderSelection();
							}

							if (ImGui::TreeNodeEx((void*)&entity, leaf_flags, "%s", entityTag.c_str()))
							{
								if (ImGui::IsItemClicked() /*&& ImGui::IsMouseReleased(0)*/)
								{
									//s_Context.s_SelectedEntity = entity;
								}
							}
							if (ImGui::IsItemHovered())
							{
								if (ImGui::IsMouseDown(0))
									preSelect = entity;
								else if (ImGui::IsMouseReleased(0))
									s_Context.m_SelectedEntity = preSelect;
							}

							UIPayload::BeginSource(PAYLOAD_DATA_TYPE_ENTITY, &entity, sizeof(entity), entityTag.c_str());

							ImGui::PopID();
						}
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
			s_Context.m_SelectedEntity = picked;
	}

	Stimpi::Entity SceneHierarchyWindow::GetSelectedEntity()
	{
		return s_Context.m_SelectedEntity;
	}

	void SceneHierarchyWindow::ComponentInspectorWidget()
	{
		if (ImGui::Begin("Component inspector", &m_ShowInspect))
		{
			ShowSelectedEntityComponents((bool)s_Context.m_SelectedEntity);
		}
		ImGui::End();
	}

	void SceneHierarchyWindow::SettingsPopupButton(ImVec2 cursorPos, std::string name, std::function<bool()> popupContent)
	{
		// Save cursor position
		ImVec2 temp = ImGui::GetCursorPos();
		cursorPos.x += ImGui::GetWindowContentRegionWidth() - ImGuiEx::GetStyle().m_IconOffset - 2;
		ImGui::SetCursorPos(cursorPos);
		static bool showSettings = false;

		std::string btnID = name.append("_IconButton");
		if (ImGuiEx::IconButton(btnID.c_str(), EDITOR_ICON_GEAR))
		{
			showSettings = true;
			ImGui::OpenPopup(name.c_str());
		}

		// Restore cursor position
		ImGui::SetCursorPos(temp);

		if (showSettings)
		{
			if (ImGui::BeginPopup(name.c_str(), ImGuiWindowFlags_NoMove))
			{
				showSettings = popupContent();
				ImGui::EndPopup();
			}
		}
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
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Quad##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_CUBE), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			ImGui::Spacing();
			ImGui::DragFloat3("Position##Quad", glm::value_ptr(component.m_Position));
			ImGui::DragFloat2("Size##Quad", glm::value_ptr(component.m_Size));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Rotation", &component.m_Rotation, 0.01);
			ImGui::PopItemWidth();
			ImGui::Spacing();
		}

		auto popupContentQuad = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<QuadComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##QuadComponentPopup", popupContentQuad);
	}

	void SceneHierarchyWindow::CircleComponentLayout(CircleComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Circle##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_CUBE), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			ImGui::Spacing();
			ImGui::DragFloat3("Position##Circle", glm::value_ptr(component.m_Position));
			ImGui::DragFloat2("Size##Circle", glm::value_ptr(component.m_Size));
			ImGui::ColorEdit4("Color##CircleColor", glm::value_ptr(component.m_Color));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Thickness", &component.m_Thickness, 0.01);
			ImGui::DragFloat("Fade", &component.m_Fade, 0.001);
			ImGui::PopItemWidth();
			ImGui::Spacing();
		}

		auto popupContentCircle = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<CircleComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##CircleComponentPopup", popupContentCircle);
	}

	void SceneHierarchyWindow::ScriptComponentLayout(ScriptComponent& component)
	{
		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Script##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_SCRIPT), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			static char scriptName[64];
			strcpy(scriptName, component.m_ScriptName.c_str());

			static bool showPopup = false;
			ImGui::Spacing();
			ImGui::InputText("##ScriptComponentPreview", scriptName, sizeof(scriptName), ImGuiInputTextFlags_ReadOnly);
			if (ImGui::IsItemClicked())
			{
				SearchPopup::OpenPopup();
				showPopup = true;
			}
			
			if (showPopup)
			{
				auto& filterData = ScriptEngine::GetScriptClassNames();
				if (SearchPopup::OnImGuiRender(filterData))
				{
					component.m_ScriptName = SearchPopup::GetSelection();
					ScriptEngine::OnScriptComponentAdd(component.m_ScriptName, s_Context.m_SelectedEntity);
					showPopup = false;
				}
			}

			// Fields & Properties
			auto scriptClass = ScriptEngine::GetScriptClassByName(component.m_ScriptName);
			if (scriptClass != nullptr)
			{
				ImGuiInputTextFlags fieldInputFlags = ImGuiInputTextFlags_EnterReturnsTrue;

				ImGui::Separator();
				std::shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(s_Context.m_SelectedEntity);
				if (scriptInstance != nullptr)
				{
					auto& fields = scriptInstance->GetFields();
					auto& tagName = s_Context.m_SelectedEntity.GetComponent<TagComponent>().m_Tag;
					for (auto& item : fields)
					{
						auto& field = item.second;
						std::string& fieldName = field->GetName();
						std::string& fieldTypeName = field->GetFieldTypeName();

						if (ScriptFieldFragment::IsFieldTypeSupported(fieldTypeName))
						{
							ScriptFieldFragment::ScriptFieldInput(scriptInstance->GetInstance().get(), field.get());
						}
						else
						{
							// FIXME: temp check to prevent fNum corruption
							if (field->GetType() != FieldType::FIELD_TYPE_CLASS && field->GetType() != FieldType::FIELD_TYPE_STRUCT)
							{
								float fNum = 0.0f;
								field->GetValue(&fNum);
								std::string label = fmt::format("##{}_{}", field->GetName(), tagName);
								if (ImGui::InputFloat(label.c_str(), &fNum, 0.0f, 0.0f, "%.3f", fieldInputFlags))
								{
									field->SetValue(&fNum);
								}
								ImGui::SameLine();
								ImGui::Text(fieldName.c_str());
							}
						}
					}
				}
			}

			ImGui::Spacing();
		}

		auto popupContentScript = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				ScriptEngine::OnScriptComponentRemove(s_Context.m_SelectedEntity);
				s_Context.m_SelectedEntity.RemoveComponent<ScriptComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##ScriptComponentPopup", popupContentScript);
	}

	void SceneHierarchyWindow::SpriteComponentLayout(SpriteComponent& component)
	{
		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Sprite##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_SPRITE), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			std::filesystem::path texturePath = component.m_FilePath;

			ImGui::Spacing();
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
			ImGui::Spacing();
		}

		auto popupContentSprite = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<SpriteComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##SpriteComponentPopup", popupContentSprite);
	}


	void SceneHierarchyWindow::SortingGroupComponentLayout(SortingGroupComponent& component)
	{
		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("SortingGroup##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_SPRITE), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			auto& sortingLayers = Project::GetSortingLayers();
			std::string currentSortingLayer = component.m_SortingLayerName;

			ImGui::Spacing();
			if (ImGui::BeginCombo("Sorting Layer", currentSortingLayer.c_str()))
			{
				for (auto& layer : sortingLayers)
				{
					bool isSelected = layer->m_Name == currentSortingLayer;
					if (ImGui::Selectable(layer->m_Name.c_str(), isSelected))
					{
						currentSortingLayer = layer->m_Name;
						component.m_SortingLayerName = layer->m_Name;
						m_ActiveScene->UpdateLayerSorting(s_Context.m_SelectedEntity);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo(); 
			}

			int orderInLayerInput = (int)component.m_OrderInLayer;
			if (ImGui::InputInt("Order in Layer##SortingGroup", &orderInLayerInput))
			{
				if (orderInLayerInput < 0)
					orderInLayerInput = 0;

				component.m_OrderInLayer = orderInLayerInput;
				m_ActiveScene->UpdateLayerSorting(s_Context.m_SelectedEntity);
			}
			EditorUtils::SetActiveItemCaptureKeyboard(false);
			ImGui::Spacing();
		}

		auto popupContentSortingGroup = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<SortingGroupComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##SortingGroupComponentPopup", popupContentSortingGroup);
	}

	void SceneHierarchyWindow::AnimatedSpriteComponentLayout(AnimatedSpriteComponent& component)
	{
		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("AnimatedSprite##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_ANIM), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			ImGui::Spacing();
			ImGui::Text("Default Animation");
			ImGui::SameLine(0.0f, 30.0f);
			if (ImGui::Button(">##AnimatedSpriteComponent_Default", ImVec2(0.0f, 20.0f)))
			{
				if (component.m_AnimSprite && component.m_DefaultAnimation)
				{
					component.m_AnimSprite->SetAnimation(component.m_DefaultAnimation);
					component.m_AnimSprite->Start();
				}
			}
			ImGui::SameLine(0.0f, 5.0f);
			std::string defaultAnimLabel = "None";
			if (component.m_DefaultAnimation)
				defaultAnimLabel = component.m_DefaultAnimation->GetName();
			ImGui::Text(defaultAnimLabel.c_str());

			UIPayload::BeginTarget(PAYLOAD_ANIMATION, [&component](void* data, uint32_t size) {
				std::string strData = std::string((char*)data, size);
				ST_CORE_INFO("Texture data dropped: {0}", strData.c_str());
				component.SetDefailtAnimation(strData);
				});

			if (ImGui::Button(">##AnimatedSpriteComponent"))
			{
				if (component.m_AnimSprite)
					component.m_AnimSprite->Start();
			}
			ImGui::SameLine();
			if (ImGui::Button("=##AnimatedSpriteComponent"))
			{
				if (component.m_AnimSprite)
					component.m_AnimSprite->Pause();
			}
			ImGui::SameLine();
			if (ImGui::Button("x##AnimatedSpriteComponent"))
			{
				if (component.m_AnimSprite)
					component.m_AnimSprite->Stop();
			}

			static bool looping = false;
			if (ImGui::Checkbox("Loop##AnimatedSpriteComponent", &looping))
			{
				if (component.m_AnimSprite)
					component.m_AnimSprite->SetLooping(looping);
			}

			float playSpeed = 1.0f;
			if (component.m_AnimSprite)
				playSpeed = component.m_AnimSprite->GetPlaybackSpeed();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(50.0f);
			if (ImGui::DragFloat("Playback speed", &playSpeed, 0.001f, 0.01f))
			{
				if (playSpeed < 0.001f) playSpeed = 0.001;
			}
			if (component.m_AnimSprite)
				component.m_AnimSprite->SetPlaybackSpeed(playSpeed);

			// Available animation list
			if (ImGui::TreeNode("Animations##AnimatedSpriteComponent"))
			{
				// Target is TreeNode
				UIPayload::BeginTarget(PAYLOAD_ANIMATION, [&component](void* data, uint32_t size) {
					std::string strData = std::string((char*)data, size);
					component.AddAnimation(strData);
					});

				ImGui::Spacing();

				uint32_t animNum = 0;
				std::string toBeRemoved = "";
				for (auto& anim : component.m_Animations)
				{
					std::string animLabel = fmt::format("Animation_{}", animNum);
					std::string buttonLabel = fmt::format(">##AnimatedSpriteComponent_{}", animNum);
					ImGui::Text("Animation");
					ImGui::SameLine(0.0f, 30.0f);
					if (ImGui::Button(buttonLabel.c_str(), ImVec2(0.0f, 20.0f)))
					{
						if (component.m_AnimSprite)
						{
							component.m_AnimSprite->SetAnimation(anim.second);
							component.m_AnimSprite->Start();
						}
					}
					ImGui::SameLine(0.0f, 5.0f);
					float availableWidth = ImGui::GetWindowContentRegionWidth() - ImGui::GetCursorPosX() - 18.0f; // 18.0f is for "..." and a button
					std::string str = EditorUtils::StringTrimByWidth(anim.second->GetName(), availableWidth);
					ImGui::Text(str.c_str());

					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 15.0f, 5.0f);
					std::string editButtonLabel = fmt::format("##EditAnimationButton_{}", animNum);
					std::string animSettingsPopup = fmt::format("##EditAnimationPopup_{}", animNum);
					static bool showAnimSettings = false;
					if (ImGuiEx::IconButton(editButtonLabel.c_str(), EDITOR_ICON_GEAR))
					{
						showAnimSettings = true;
						ImGui::OpenPopup(animSettingsPopup.c_str());
					}

					auto animSettingsPopupContent = [&anim, &toBeRemoved]() -> bool
					{
						bool showPoput = true;
						if (ImGui::Selectable("Remove"))
						{
							showPoput = false;
							toBeRemoved = anim.first;
						}

						return showPoput;
					};

					if (showAnimSettings)
					{
						if (ImGui::BeginPopup(animSettingsPopup.c_str(), ImGuiWindowFlags_NoMove))
						{
							showAnimSettings = animSettingsPopupContent();
							ImGui::EndPopup();
						}
					}

					animNum++;
				}

				// Finish cleanup, if Animation was selected for removal by popup
				if (toBeRemoved != "")
				{
					component.RemoveAnimation(toBeRemoved);
				}

				ImGui::TreePop();
			}
			else
			{
				UIPayload::BeginTarget(PAYLOAD_ANIMATION, [&component](void* data, uint32_t size) {
					std::string strData = std::string((char*)data, size);
					component.AddAnimation(strData);
					});
			}

			ImGui::Spacing();
		}

		auto popupContentAnimatedSprite = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<AnimatedSpriteComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##AnimatedSpriteComponentPopup", popupContentAnimatedSprite);
	}

	void SceneHierarchyWindow::CameraComponentLayout(CameraComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
		glm::vec4 view = component.m_Camera->GetOrthoCamera()->GetViewQuad();

		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Camera##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_CAM), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			ImGui::Spacing();
			ImGui::Checkbox("Main##Camera", &component.m_IsMain);

			ImGui::Text("Camera Viewport:");
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("X##CameraComponent left input", &view.x);

			ImGui::SameLine();
			ImGui::DragFloat("Y##CameraComponent bottom input", &view.z);

			ImGui::DragFloat("W##CameraComponent right input", &view.y);

			ImGui::SameLine();
			ImGui::DragFloat("H##CameraComponent top input", &view.w);
			ImGui::Spacing();

			component.m_Camera->SetOrthoView(view);
		}

		auto popupContentCamera = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<CameraComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##CameraComponentPopup", popupContentCamera);
	}

	void SceneHierarchyWindow::RigidBody2DComponentLayout(RigidBody2DComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Rigid Body 2D##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_RB2D), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.m_Type];

			ImGui::Spacing();
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
			ImGui::Spacing();
		}

		auto popupContentRigidBody2D = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<RigidBody2DComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##RigidBody2DComponentPopup", popupContentRigidBody2D);
	}

	void SceneHierarchyWindow::BoxCollider2DComponentLayout(BoxCollider2DComponent& component)
	{
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::Separator();
		ImVec2 cursor = ImGui::GetCursorPos();	// To render SettingsPopupButton on the same line as Collapsing header

		if (ImGui::CollapsingHeaderIcon("Box Collider##ComponentName", EditorResources::GetIconTextureID(EDITOR_ICON_WCUBE), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
		{
			const char* colliderShapeStrings[] = { "Box", "Circle" };
			const char* currentColliderShapeString = colliderShapeStrings[(int)component.m_ColliderShape];

			ImGui::Spacing();
			if (ImGui::BeginCombo("Collider Shape", currentColliderShapeString))
			{
				for (int i = 0; i <= 1; i++)
				{
					bool isSelected = currentColliderShapeString == colliderShapeStrings[i];
					if (ImGui::Selectable(colliderShapeStrings[i], isSelected))
					{
						currentColliderShapeString = colliderShapeStrings[i];
						component.m_ColliderShape = (BoxCollider2DComponent::Collider2DShape)i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::DragFloat2("Offset", glm::value_ptr(component.m_Offset));
			ImGui::DragFloat2("Size##Collider", glm::value_ptr(component.m_Size));
			ImGui::PushItemWidth(80.0f);
			ImGui::DragFloat("Density", &component.m_Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.m_Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.m_Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.m_RestitutionThreshold, 0.01f, 0.0f);
			ImGui::PopItemWidth();
			ImGui::Spacing();
		}

		auto popupContentBoxCollider = []() -> bool
		{
			bool showPoput = true;
			if (ImGui::Selectable("Remove"))
			{
				showPoput = false;
				s_Context.m_SelectedEntity.RemoveComponent<BoxCollider2DComponent>();
			}

			return showPoput;
		};

		SettingsPopupButton(cursor, "##BoxColliderComponentPopup", popupContentBoxCollider);
	}

	void SceneHierarchyWindow::AddComponentLayout()
	{
		ImGuiComboFlags flags = ImGuiComboFlags_PopupAlignLeft | ImGuiComboFlags_NoArrowButton;
		const char* selectedPreview = "Add Component";

		ImGui::PushItemWidth(100.0f);
		if (ImGui::BeginCombo("##AddComponentWidget", selectedPreview, flags))
		{
			if (!s_Context.m_SelectedEntity.HasComponent<QuadComponent>())
			{
				if (ImGui::Selectable("Quad##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<QuadComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<CircleComponent>())
			{
				if (ImGui::Selectable("Circle##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<CircleComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<SpriteComponent>())
			{
				if (ImGui::Selectable("Sprite##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<SpriteComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<SortingGroupComponent>())
			{
				if (ImGui::Selectable("SortingGroup##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<SortingGroupComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<AnimatedSpriteComponent>())
			{
				if (ImGui::Selectable("AnimatedSprite##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<AnimatedSpriteComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<ScriptComponent>())
			{
				if (ImGui::Selectable("Script##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<ScriptComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<CameraComponent>())
			{
				if (ImGui::Selectable("Camera##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<RigidBody2DComponent>())
			{
				if (ImGui::Selectable("RigidBody2D##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<RigidBody2DComponent>();
				}
			}

			if (!s_Context.m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::Selectable("BoxCollider2D##AddComponent"))
				{
					s_Context.m_SelectedEntity.AddComponent<BoxCollider2DComponent>();
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
			if (s_Context.m_SelectedEntity.HasComponent<TagComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<TagComponent>();
				TagComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<QuadComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<QuadComponent>();
				QuadComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<CircleComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<CircleComponent>();
				CircleComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<SpriteComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<SpriteComponent>();
				SpriteComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<SortingGroupComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<SortingGroupComponent>();
				SortingGroupComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<AnimatedSpriteComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<AnimatedSpriteComponent>();
				AnimatedSpriteComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<ScriptComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<ScriptComponent>();
				ScriptComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<CameraComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<CameraComponent>();
				CameraComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<RigidBody2DComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<RigidBody2DComponent>();
				RigidBody2DComponentLayout(component);
			}

			if (s_Context.m_SelectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				auto& component = s_Context.m_SelectedEntity.GetComponent<BoxCollider2DComponent>();
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
		// For now keep this disabled and use "+" button instead
		/*if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("CreateEntityPopup");
		}*/

		if (ImGui::BeginPopup("CreateEntityPopup"))
		{
			if (ImGui::Selectable("Empty Entity"))
			{
				s_Context.m_SelectedEntity = m_ActiveScene->CreateEntity("NewEntity");
			}

			if (ImGui::BeginMenu("Scene"))
			{
				if (ImGui::MenuItem("Camera"))
				{
					s_Context.m_SelectedEntity = m_ActiveScene->CreateEntity("Camera_Object");
					s_Context.m_SelectedEntity.AddComponent<QuadComponent>();
					s_Context.m_SelectedEntity.AddComponent<CameraComponent>(std::make_shared<Camera>(), false);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Primitive"))
			{
				if (ImGui::MenuItem("Sprite"))
				{
					s_Context.m_SelectedEntity = m_ActiveScene->CreateEntity("Sprite_Object");
					s_Context.m_SelectedEntity.AddComponent<QuadComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f), 0.0f);
					s_Context.m_SelectedEntity.AddComponent<SpriteComponent>();
				}

				if (ImGui::MenuItem("Circle"))
				{
					s_Context.m_SelectedEntity = m_ActiveScene->CreateEntity("Circle_Object");
					s_Context.m_SelectedEntity.AddComponent<CircleComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f));
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}

}