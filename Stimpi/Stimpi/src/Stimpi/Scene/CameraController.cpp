#include "stpch.h"
#include "Stimpi/Scene/CameraController.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	CameraController::CameraController(Camera* camera)
		: m_Camera(camera), m_MouseActive(false)
	{
		m_MouseHandler = std::make_shared<MouseEvnetHandler>([&](MouseEvent e) -> bool {
			float scrollSpeed = 20.f;
			float zoomFactor = 2.0f;

			static float width = 1280.0f;
			static float height = 720.0f;

			// If not active exit
			if (!m_MouseActive)
				return false;

			// Zoom on scroll - resizing Camera viewport
			if (InputManager::Instance()->IsKeyPressed(ST_KEY_LCTRL) && ((e.GetType() == MouseEventType::MOUSE_EVENT_WHEELUP) || (e.GetType() == MouseEventType::MOUSE_EVENT_WHEELDOWN)))
			{
				width += scrollSpeed * e.GetScrollY();
				m_Camera->SetZoomFactor(m_Camera->GetViewportWidth() / width);
			}

			static bool buttonHold = false;
			static glm::vec2 startPosition;
			static glm::vec2 movePosition;
			static glm::vec3 cameraStartPos;
			if (InputManager::Instance()->IsMouseButtonPressed(ST_BUTTON_MIDDLE))
			{
				if (!buttonHold)	// Start of MMB hold
				{
					startPosition = glm::vec2(e.GetX(), e.GetY());
					cameraStartPos = m_Camera->GetPosition();
				}
				movePosition = startPosition - glm::vec2(e.GetX(), e.GetY());
				m_Camera->SetPosition(cameraStartPos + glm::vec3{ movePosition.x, -movePosition.y, 0.0f });
				buttonHold = true;
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
		if (m_Camera != nullptr)
		{
			static float moveSpeed = 400.0f;
			static float zoomSpeed = 10.0f;

			// Movement WASD
			if (InputManager::IsKeyPressed(ST_KEY_W))
				m_Camera->Translate({ 0.0f, moveSpeed * ts, 0.0f });
			if (InputManager::IsKeyPressed(ST_KEY_S))
				m_Camera->Translate({ 0.0f, -moveSpeed * ts, 0.0f });
			if (InputManager::IsKeyPressed(ST_KEY_D))
				m_Camera->Translate({ moveSpeed * ts, 0.0f, 0.0f });
			if (InputManager::IsKeyPressed(ST_KEY_A))
				m_Camera->Translate({ -moveSpeed * ts, 0.0f, 0.0f });
		}
	}
}