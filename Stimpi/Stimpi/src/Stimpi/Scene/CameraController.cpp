#include "stpch.h"
#include "Stimpi/Scene/CameraController.h"

namespace Stimpi
{

	CameraController::CameraController(Camera* camera)
		: m_Camera(camera)
	{
		m_MouseHandler = std::make_shared<MouseEvnetHandler>([&](MouseEvent e) -> bool {
			float scrollSpeed = 20.f;
			float zoomFactor = 2.0f;

			static float width = 1280.0f;
			static float height = 720.0f;

			if ((e.GetType() == MouseEventType::MOUSE_EVENT_WHEELUP) || (e.GetType() == MouseEventType::MOUSE_EVENT_WHEELDOWN))
			{
				//m_Camera->Translate({ 0.0f, scrollSpeed * e.GetScrollY(), 0.0f });
				width += scrollSpeed * e.GetScrollY();
				height += scrollSpeed * e.GetScrollY();
				if (width / 1.778f > height)
					m_Camera->Resize(0.0f, width, 0.0f, width / 1.778f);
				else
					m_Camera->Resize(0.0f, height*1.778f, 0.0f, height);
			}
			return false;
			});
		InputManager::Instance()->AddMouseEventHandler(m_MouseHandler.get());
	}

	CameraController::~CameraController()
	{
		InputManager::Instance()->RemoveMouseEventHandler(m_MouseHandler.get());
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

			// Zoom mouse scroll
		}
	}
}