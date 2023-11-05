#include "stpch.h"
#include "Stimpi/Gui/SceneViewWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Gui/Components/UIPayload.h"
#include "Stimpi/Gui/SceneHierarchyWindow.h"
#include "Stimpi/Gui/Gizmo2D.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"


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
		/*if (((float)ws.x / 1.7778f) >= (float)ws.y)
		{
			frameBuffer->Resize(ws.x, ((float)ws.x / 1.7778f));
		}
		else
		{
			frameBuffer->Resize(((float)ws.y*1.778f), ws.y);
		}*/

		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseClicked[ImGuiMouseButton_Left] && m_Hovered)
		{
			ImVec2 winPos = ImGui::GetCursorScreenPos();
			ImVec2 clickPos = io.MouseClickedPos[ImGuiMouseButton_Left];
			ImVec2 pickPos = { clickPos.x - winPos.x, ws.y - (clickPos.y - winPos.y) };

			auto camPos = camera->GetPosition();
			float zoomFactor = camera->GetZoomFactor();

			auto picked = scene->MousePickEntity(zoomFactor * pickPos.x + camPos.x, zoomFactor * pickPos.y + camPos.y);
			//ST_CORE_INFO("Picked Entity: {0}", (uint32_t)picked);
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
			SceneManager::Instance()->LoadScene(strData);	// TODO: investigate app stuck
			});


		ImGui::End();
	}

}