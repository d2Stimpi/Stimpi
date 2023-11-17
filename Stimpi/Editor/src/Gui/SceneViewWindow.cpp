#include "stpch.h"
#include "Gui/SceneViewWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"
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
			ImVec2 winPos = ImGui::GetCursorScreenPos();
			ImVec2 clickPos = io.MouseClickedPos[ImGuiMouseButton_Left];
			glm::vec2 pickPos = { clickPos.x - winPos.x, ws.y - (clickPos.y - winPos.y) };

			ST_CORE_INFO("Win pos {0}", pickPos);

			auto worldPos = SceneUtils::WindowToWorldPoint(camera, glm::vec2{ ws.x, ws.y }, pickPos);
			auto windowPos = SceneUtils::WorldToWindowPoint(camera, glm::vec2{ ws.x, ws.y }, worldPos);

			ST_CORE_INFO("World pos {0}", worldPos);
			ST_CORE_INFO("Camera pos {0}", camera->GetPosition());
			ST_CORE_INFO("Window pos {0} \n", windowPos);

			auto picked = scene->MousePickEntity(worldPos.x, worldPos.y);

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