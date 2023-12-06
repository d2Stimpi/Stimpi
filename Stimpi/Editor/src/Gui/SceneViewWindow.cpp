#include "stpch.h"
#include "Gui/SceneViewWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/KeyCodes.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Gui/Components/UIPayload.h"
#include "Gui/SceneHierarchyWindow.h"
#include "Gui/Gizmo2D.h"
#include "Gui/EditorUtils.h"


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
		auto frameBuffer = Renderer2D::Instace()->GetFrameBuffer();
		auto scene = SceneManager::Instance()->GetActiveScene();
		auto camera = scene->GetRenderCamera();

		ImGui::Begin("OpenGL Main Scene View", &m_Show, m_Flags);
		ImGui::BeginChild("##DropTarget-SceneView");	// Used to be able to catch drag-drop item on whole window

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();
		// Invert the image on ImGui window
		ImVec2 uv_min = ImVec2(0.0f, winSize.y / frameBuffer->GetHeight());
		ImVec2 uv_max = ImVec2(winSize.x / frameBuffer->GetWidth(), 0.0f);

		m_Hovered = ImGui::IsWindowHovered();
		m_Focused = ImGui::IsWindowFocused();

		// Aspect ratio 16:9
		// TODO: configurable aspect ratio
		if (((float)winSize.x / 1.7778f) >= (float)winSize.y)
		{
			frameBuffer->Resize(winSize.x, ((float)winSize.x / 1.7778f));
		}
		else
		{
			frameBuffer->Resize(((float)winSize.y*1.778f), winSize.y);
		}

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
			static GizmoAction action = GizmoAction::NONE;
			if (EditorUtils::WantCaptureKeyboard())
			{
				if (InputManager::Instance()->IsKeyPressed(ST_KEY_Q))
				{
					action = GizmoAction::NONE;
				}
				if (InputManager::Instance()->IsKeyPressed(ST_KEY_W))
				{
					action = GizmoAction::TRANSLATE;
				}
				if (InputManager::Instance()->IsKeyPressed(ST_KEY_E))
				{
					action = GizmoAction::SCALE;
				}
			}

			if (selectedEntity.HasComponent<QuadComponent>())
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


		ImGui::End();
	}

	void SceneViewWindow::DrawUIComponents(ImVec2 winPos, ImVec2 winSize)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		auto sceneCamera = scene->GetRenderCamera();

		scene->m_Registry.view<CameraComponent>().each([=, &scene](auto e, CameraComponent& camera)
			{
				static Texture* camTexture = ResourceManager::Instance()->LoadTexture("..\/Editor\/assets\/icons\/camera.png");
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

}