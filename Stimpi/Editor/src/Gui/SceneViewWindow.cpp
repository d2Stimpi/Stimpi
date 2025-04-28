#include "stpch.h"
#include "Gui/SceneViewWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/KeyCodes.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Asset/TextureImporter.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Gui/Components/UIPayload.h"
#include "Gui/Components/Toolbar.h"
#include "Gui/Panels/SceneHierarchyWindow.h"
#include "Gui/Gizmo2D.h"
#include "Gui/EditorUtils.h"
#include "Gui/Utils/Utils.h"
#include "Gui/Prefab/PrefabManager.h"

#include <cmath>

namespace Stimpi
{

	SceneViewWindow::SceneViewWindow()
	{
		m_Flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;

		InputManager::Instance()->AddKeyboardEventHandler(new KeyboardEventHandler([](KeyboardEvent event) -> bool {
				if (InputManager::Instance()->IsKeyPressed(ST_KEY_LCTRL) && event.GetKeyCode() == ST_KEY_C && event.GetType() == KeyboardEventType::KEY_EVENT_DOWN )
				{
					auto scene = SceneManager::Instance()->GetActiveScene();
					if (scene->GetRuntimeState() == RuntimeState::STOPPED)
					{
						if (scene != nullptr && EditorUtils::WantCaptureKeyboard())
						{
							auto selectedEntity = SceneHierarchyWindow::GetSelectedEntity();
							if (selectedEntity)
							{
								auto newEntity = scene->CopyEntity(selectedEntity);
								SceneHierarchyWindow::SetPickedEntity(newEntity);
							}
						}
					}
				}
				return false;
			}));
	}

	SceneViewWindow::~SceneViewWindow()
	{

	}

	void SceneViewWindow::OnImGuiRender()
	{
		auto frameBuffer = Renderer2D::Instance()->GetFrameBuffer();
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto camera = scene->GetRenderCamera();
		ST_CORE_ASSERT(!camera);

		// Gizmo control action selection
		static GizmoAction action = GizmoAction::NONE;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
		ImGui::Begin("Scene View", &m_Show, m_Flags);
		ImGui::PopStyleVar();

		ToolbarStyle toolbarStyle;
		toolbarStyle.m_Size = { 30.0f, 21.0f };
		toolbarStyle.m_IconSize = s_DefaultSmallIconSize;

		Toolbar::PushStyle(toolbarStyle);
		Toolbar::Begin("SceneViewToolbar");
		{
			if (Toolbar::ToolbarToggleIconButton("##Free_SceneViewToolbar", EDITOR_ICON_HAND, action == GizmoAction::NONE))
			{
				action = GizmoAction::NONE;
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarToggleIconButton("##Translate_SceneViewToolbar", EDITOR_ICON_MOVE, action == GizmoAction::TRANSLATE))
			{
				action = GizmoAction::TRANSLATE;
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarToggleIconButton("##Scale_SceneViewToolbar", EDITOR_ICON_SCALE, action == GizmoAction::SCALE))
			{
				action = GizmoAction::SCALE;
			}
			Toolbar::Separator();
		}
		Toolbar::End();
		Toolbar::PopStyle();

		ImGui::BeginChild("##DropTarget-SceneView");	// Used to be able to catch drag-drop item on whole window

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();
		// Invert the image on ImGui window
		ImVec2 uv_min = ImVec2(0.0f, winSize.y / frameBuffer->GetHeight());
		ImVec2 uv_max = ImVec2(winSize.x / frameBuffer->GetWidth(), 0.0f);

		m_Hovered = ImGui::IsWindowHovered();
		m_Focused = ImGui::IsWindowFocused();

		if (scene->GetRuntimeState() == RuntimeState::RUNNING)
		{
			if (!m_Hovered)	InputManager::Instance()->SetCaptureMouse(false);
			if (!m_Focused) InputManager::Instance()->SetCaptureKeyboard(false);
		}

		m_WindowSize = winSize;
		m_WindowPosition = pos;
		// Limit mouse position to screen
		m_MousePosition = ImGui::GetMousePos();
		glm::vec2 bound = SceneUtils::SquareBoundPosition({ winSize.x, winSize.y }, { m_MousePosition.x - pos.x, m_MousePosition.y - pos.y });
		m_MousePosition = ImVec2(bound.x, bound.y);

		// Aspect ratio 16:9
		// TODO: configurable aspect ratio
		if (((float)winSize.x / 1.7778f) >= (float)winSize.y)
		{
			frameBuffer->Resize(winSize.x, std::round((float)winSize.x / 1.7778f));
		}
		else
		{
			frameBuffer->Resize(std::round((float)winSize.y*1.778f), winSize.y);
		}

		// No "Editor" mouse picking when in Running Scene state
		if (scene->GetRuntimeState() != RuntimeState::RUNNING)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (io.MouseClicked[ImGuiMouseButton_Left] && m_Hovered && (Gizmo2D::IsUsing() == false))
			{
				ImVec2 winPos = ImGui::GetCursorScreenPos();
				ImVec2 clickPos = io.MouseClickedPos[ImGuiMouseButton_Left];
				glm::vec2 pickPos = { clickPos.x - winPos.x, winSize.y - (clickPos.y - winPos.y) };

				auto worldPos = SceneUtils::WindowToWorldPoint(camera, glm::vec2{ winSize.x, winSize.y }, pickPos);

				// First try to pick UI components only
				if (PickUIComponents(ImVec2(pickPos.x, pickPos.y)) == false)
				{
					auto picked = scene->MousePickEntity(worldPos.x, worldPos.y);
					// Pass picked Entity to SceneHierarchy panel
					SceneHierarchyWindow::SetPickedEntity(picked);
				}
			}
		}

		// Draw Scene before Gizmo
		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + winSize.x, pos.y + winSize.y), uv_min, uv_max);
		
		// Draw Scene UI components, Cameras, etc...
		if (scene->GetRuntimeState() == RuntimeState::STOPPED)
		{
			DrawUIComponents(pos, winSize);
		}

		// Draw Gizmo last and on top
		auto selectedEntity = SceneHierarchyWindow::GetSelectedEntity();
		if (selectedEntity && scene->GetRuntimeState() != RuntimeState::RUNNING)
		{
			if (EditorUtils::WantCaptureKeyboard())
			{
				if (InputManager::Instance()->IsKeyDown(ST_KEY_Q))
				{
					action = GizmoAction::NONE;
				}
				if (InputManager::Instance()->IsKeyDown(ST_KEY_W))
				{
					action = GizmoAction::TRANSLATE;
				}
				if (InputManager::Instance()->IsKeyDown(ST_KEY_E))
				{
					action = GizmoAction::SCALE;
				}
			}

			if (selectedEntity.HasComponent<QuadComponent>() || selectedEntity.HasComponent<CircleComponent>())
			{
				Gizmo2D::SetDrawlist(ImGui::GetWindowDrawList());
				Gizmo2D::Manipulate(camera, selectedEntity, action);
			}
		}

		ImGui::EndChild();

		// Accept Scene as drag-drop item to load it
		UIPayload::BeginTarget(PAYLOAD_SCENE, [](void* data, uint32_t size) {
			std::string strData = std::string((char*)data, size);
			ST_CORE_INFO("Scene data dropped: {0}", strData.c_str());
			SceneManager::Instance()->LoadScene(strData);
		});

		UIPayload::BeginTarget(PAYLOAD_PREFAB, [&](void* data, uint32_t size) {
			AssetHandle prefabHandle = *(AssetHandle*)data;
			Entity prefabEntity = PrefabManager::InstantiatePrefab(prefabHandle);

			if (prefabEntity && prefabEntity.HasComponent<QuadComponent>())
			{
				ImVec2 worldPos = GetWorldMousePosition();
				QuadComponent& quad = prefabEntity.GetComponent<QuadComponent>();
				quad.m_Position.x = worldPos.x;
				quad.m_Position.y = worldPos.y;
			}
		});

		ImGui::End();
	}

	void SceneViewWindow::DrawUIComponents(ImVec2 winPos, ImVec2 winSize)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		auto sceneCamera = scene->GetRenderCamera();

		scene->m_Registry.view<CameraComponent>().each([=, &scene](auto e, CameraComponent& camera)
			{
				static std::shared_ptr<Texture> camTexture = TextureImporter::LoadTexture(Project::GetAssestsDir() / "textures\/camera.png");
				glm::vec2 iconSize = { 30.0f , 24.0f }; // TODO: Icon manager? and uniform size of icons
				auto camPos = camera.m_Camera->GetPosition();
				auto camView = camera.m_Camera->GetOrthoView();

				// Position Camera's UI icon by Quad Center if it exists
				Entity entity = scene->GetEntityByHandle(e);
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();
					auto quadCenter = quad.Center();
					camPos = { quadCenter.x, quadCenter.y, camPos.z };
				}

				// Cam Icon
				auto calcPos = SceneUtils::WorldToWindowPoint(sceneCamera, glm::vec2{ winSize.x, winSize.y }, { camView.x + camPos.x, camView.z + camPos.y });
				calcPos = EditorUtils::PositionInCurentWindow(calcPos);
				ImVec2 uv_min = ImVec2(0.0f, 1.0f);
				ImVec2 uv_max = ImVec2(1.0f, 0.0f);
				if (camTexture)
					ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)camTexture->GetTextureID(), ImVec2(calcPos.x - iconSize.x / 2, calcPos.y - iconSize.y / 2), ImVec2(calcPos.x + iconSize.x / 2, calcPos.y + iconSize.y / 2), uv_min, uv_max);
				
				// Cam bounds
				camPos = camera.m_Camera->GetPosition(); // Reset Camera position
				auto camMin = SceneUtils::WorldToWindowPoint(sceneCamera, glm::vec2{ winSize.x, winSize.y }, { camView.x + camPos.x, camView.z + camPos.y });
				auto camMax = SceneUtils::WorldToWindowPoint(sceneCamera, glm::vec2{ winSize.x, winSize.y }, { camView.y + camPos.x, camView.w + camPos.y });
				camMin = EditorUtils::PositionInCurentWindow(camMin);
				camMax = EditorUtils::PositionInCurentWindow(camMax);
				ImGui::GetWindowDrawList()->AddQuad(ImVec2(camMin.x, camMin.y), ImVec2(camMax.x, camMin.y), ImVec2(camMax.x, camMax.y), ImVec2(camMin.x, camMax.y), ImColor(0.8f, 0.8f, 0.2f, 1.0f), 3);
			});
	}

	bool SceneViewWindow::PickUIComponents(ImVec2 pickPos)
	{
		bool picked = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		auto sceneCamera = scene->GetRenderCamera();

		glm::vec2 iconSize = { 30.0f , 24.0f }; // TODO: Icon manager? and uniform size of icons
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();

		scene->m_Registry.view<CameraComponent>().each([&](auto e, CameraComponent& camera)
			{
				auto camPos = camera.m_Camera->GetPosition();
				auto camView = camera.m_Camera->GetOrthoView();

				// Camera's UI icon relative to Quad Center if it exists
				Entity entity = scene->GetEntityByHandle(e);
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();
					auto quadCenter = quad.Center();
					camPos = { quadCenter.x, quadCenter.y, camPos.z };
				}

				auto calcPos = SceneUtils::WorldToWindowPoint(sceneCamera, glm::vec2{ winSize.x, winSize.y }, glm::vec2{ camView.x + camPos.x, camView.z + camPos.y });

				glm::vec2 clickPos = { pickPos.x, pickPos.y };
				glm::vec2 min = { calcPos.x - iconSize.x / 2.0f, calcPos.y - iconSize.y / 2.0f };
				glm::vec2 max = { calcPos.x + iconSize.x / 2.0f, calcPos.y + iconSize.y / 2.0f };

				if (SceneUtils::IsContainedInSquare(clickPos, min, max))
				{
					SceneHierarchyWindow::SetPickedEntity(entity);
					picked = true;
				}
			});

		return picked;
	}

	ImVec2 SceneViewWindow::GetWorldMousePosition()
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto camera = scene->GetRenderCamera();
		ST_CORE_ASSERT(!camera);

		ImVec2 winSize = ImGui::GetContentRegionAvail();
		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 mousePos = ImGui::GetMousePos();
		glm::vec2 pickPos = { mousePos.x - winPos.x, winSize.y - (mousePos.y - winPos.y) };
		glm::vec2 worldPos = SceneUtils::WindowToWorldPoint(camera, glm::vec2{ winSize.x, winSize.y }, pickPos);

		return ImVec2(worldPos.x, worldPos.y);
	}

}