#include "stpch.h"
#include "Gui/SceneViewWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Gui/Components/UIPayload.h"
#include "Gui/SceneHierarchyWindow.h"
#include "Gui/Gizmo2D.h"


namespace Stimpi
{
	SceneViewWindow::SceneViewWindow()
	{
		m_Flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
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
		ImVec2 ws = ImGui::GetContentRegionAvail();
		// Invert the image on ImGui window
		ImVec2 uv_min = ImVec2(0.0f, ws.y / frameBuffer->GetHeight());
		ImVec2 uv_max = ImVec2(ws.x / frameBuffer->GetWidth(), 0.0f);

		m_Hovered = ImGui::IsWindowHovered();
		m_Focused = ImGui::IsWindowFocused();

		// Aspect ratio 16:9
		// TODO: configurable aspect ratio
		if (((float)ws.x / 1.7778f) >= (float)ws.y)
		{
			frameBuffer->Resize(ws.x, ((float)ws.x / 1.7778f));
		}
		else
		{
			frameBuffer->Resize(((float)ws.y*1.778f), ws.y);
		}

		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseClicked[ImGuiMouseButton_Left] && m_Hovered && (Gizmo2D::IsUsing() == false))
		{
			/* Calculating the Picking position
			* 1. Determine the position of click in the SceneView Window - pickPos
			* 2. Read the Camera view bounding box and multiply by camera's zoom
			* 3. Calculate displayed Camera's region. Camera view divided by ratio of FB size/window size - camRegion
			* 4. Figure out Window to World scale size - scale
			* 5. Final World pick position is calculated by adding Camera position to scaled window click position (pickPos)
			*/
			ImVec2 winPos = ImGui::GetCursorScreenPos();
			ImVec2 clickPos = io.MouseClickedPos[ImGuiMouseButton_Left];
			ImVec2 pickPos = { clickPos.x - winPos.x, ws.y - (clickPos.y - winPos.y) };
			ST_CORE_INFO("==============================");
			ST_CORE_INFO("Window size: {0}, {1}", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ST_CORE_INFO("Frame Buffer size: {0}, {1}", frameBuffer->GetWidth(), frameBuffer->GetHeight());
			ST_CORE_INFO("Scene View click position: {0}, {1}", pickPos.x, pickPos.y);

			auto camPos = camera->GetPosition();
			float zoomFactor = camera->GetZoomFactor();
			glm::vec4 camView = camera->GetOrthoView() * zoomFactor;
			glm::vec2 camRegion = { camView.y / (frameBuffer->GetWidth() / ws.x), camView.w / (frameBuffer->GetHeight() / ws.y) };
			ST_CORE_INFO("Camera view: {0}, {1}, {2}, {3}", camView.x, camRegion.x, camView.z, camRegion.y);
			glm::vec2 scale = { ws.x / camRegion.x, ws.y / camRegion.y };
			ST_CORE_INFO("Window -> World scale {0}, {1}", scale.x, scale.y);
			ST_CORE_INFO("World click position: {0}, {1}", pickPos.x / scale.x + camPos.x, pickPos.y / scale.y + camPos.y);
			ST_CORE_INFO("==============================");

			//auto picked = scene->MousePickEntity(zoomFactor * pickPos.x + camPos.x, zoomFactor * pickPos.y + camPos.y);
			auto picked = scene->MousePickEntity(pickPos.x / scale.x + camPos.x, pickPos.y / scale.y + camPos.y);

			// Pass picked Entity to SceneHierarchy panel
			SceneHierarchyWindow::SetPickedEntity(picked);
		}

		// Draw Scene before Gizmo
		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + ws.x, pos.y + ws.y), uv_min, uv_max);
		
		// Draw Gizmo
		auto selectedEntity = SceneHierarchyWindow::GetSelectedEntity();
		if (selectedEntity && scene->GetRuntimeState() != RuntimeState::RUNNING)
		{
			static GizmoAction action = GizmoAction::NONE;
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

}