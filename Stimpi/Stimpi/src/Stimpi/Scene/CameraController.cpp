#include "stpch.h"
#include "Stimpi/Scene/CameraController.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Log.h"

// TODO: Cleanup

namespace Stimpi
{
	float s_ScaleBase = 1280.0f; // Based on window size
	float s_ControllerScale = 1.0f;
	float s_ScrollZoomDelta = 0.1f;

	CameraController::CameraController(Camera* camera)
		: m_Camera(camera), m_MouseActive(false)
	{
		s_ControllerScale = m_Camera->GetOrthoView().y / s_ScaleBase;

		m_MouseHandler = std::make_shared<MouseEvnetHandler>([&](MouseEvent e) -> bool {
			// If not active exit
			if (!m_MouseActive)
				return false;

			static bool buttonHold = false;
			static bool zoomHold = false;
			static glm::vec2 startPosition;
			static glm::vec2 movePosition;

			static float startZoom;
			static float cameraZoom;
			if (InputManager::Instance()->IsKeyPressed(ST_KEY_LALT) && InputManager::Instance()->IsMouseButtonPressed(ST_BUTTON_LEFT) && buttonHold == false)
			{
				if (!zoomHold)
				{
					startPosition = glm::vec2(e.GetX(), e.GetY());
					startZoom = m_Camera->GetZoomFactor();
				}
				movePosition = startPosition - glm::vec2(e.GetX(), e.GetY());
				//ST_CORE_INFO("Move ammount {0}, {1}", movePosition.x, movePosition.y);
				movePosition = movePosition  * s_ControllerScale;
				//ST_CORE_INFO("Move ammount post scale and zoom apply {0}, {1}", movePosition.x, movePosition.y);
				m_Camera->SetZoomFactor(startZoom + movePosition.x * 0.05f);
				//ST_CORE_INFO("New zoom value {0}", m_Camera->GetZoomFactor());
				zoomHold = true;
			}
			else
			{
				s_ControllerScale = m_Camera->GetOrthoView().y / s_ScaleBase;
				// Reset zoom move state
				zoomHold = false;
			}

			// Mouse scroll zoom
			if ((e.GetType() == MouseEventType::MOUSE_EVENT_WHEELUP || e.GetType() == MouseEventType::MOUSE_EVENT_WHEELDOWN) && zoomHold == false && buttonHold == false)
			{
				m_Camera->SetZoomFactor(m_Camera->GetZoomFactor() + s_ScrollZoomDelta * (-e.GetScrollY()));
			}

			static glm::vec3 cameraStartPos;
			if (InputManager::Instance()->IsMouseButtonPressed(ST_BUTTON_MIDDLE) && zoomHold == false)
			{
				if (!buttonHold)	// Start of MMB hold
				{
					startPosition = glm::vec2(e.GetX(), e.GetY());
					cameraStartPos = m_Camera->GetPosition();
				}

				if ((e.GetType() != MouseEventType::MOUSE_EVENT_WHEELUP && e.GetType() != MouseEventType::MOUSE_EVENT_WHEELDOWN))
				{
					movePosition = startPosition - glm::vec2(e.GetX(), e.GetY());
					movePosition = movePosition * m_Camera->GetZoomFactor() * s_ControllerScale;
					m_Camera->SetPosition(cameraStartPos + glm::vec3{ movePosition.x, -movePosition.y, 0.0f });
					buttonHold = true;

					//ST_CORE_INFO("Event: {}, {} - type {}", e.GetX(), e.GetY(), (int)e.GetType());
					//ST_CORE_INFO("Move pos: {}", movePosition);
				}
			}
			else
			{
				// Reset drag move state
				buttonHold = false;
			}

			return false;
		});
		InputManager::Instance()->AddMouseEventHandler(m_MouseHandler.get());
	}

	CameraController::~CameraController()
	{
		InputManager::Instance()->RemoveMouseEventHandler(m_MouseHandler.get());
	}

	void CameraController::AttachCamera(Camera* camera)
	{
		m_Camera = camera;
	}

	void CameraController::Update(Timestep ts)
	{
		
	}
}