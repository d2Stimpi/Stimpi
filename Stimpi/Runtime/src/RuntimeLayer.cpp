#include "stpch.h"
#include "RuntimeLayer.h"

#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/SceneManager.h"

namespace Stimpi
{

	RuntimeLayer::RuntimeLayer()
	{
		m_EditorCamera = std::make_shared<Camera>(0.0f, 128.0f, 0.0f, 72.0f);
		m_EditorCamera->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_CameraController = std::make_shared<CameraController>(m_EditorCamera.get());

		SceneManager::Instance()->LoadStartingScene();
		OnSceneChangedListener onSceneChanged = [&]()
		{
			Renderer2D::Instance()->ClearScene();

			m_Scene = SceneManager::Instance()->GetActiveSceneRef();
			m_Scene->SetCamera(m_EditorCamera.get());
			//m_Scene->OnScenePlay();
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onSceneChanged);

		m_Scene = SceneManager::Instance()->GetActiveSceneRef();
		m_Scene->SetCamera(m_EditorCamera.get());
		//m_Scene->OnScenePlay();
	}

	RuntimeLayer::~RuntimeLayer()
	{

	}

	void RuntimeLayer::OnAttach()
	{
		Renderer2D::Instance()->EnableLocalRendering(true);
	}

	void RuntimeLayer::OnDetach()
	{

	}

	void RuntimeLayer::Update(Timestep ts)
	{
		// Camera movement update - only when in Stopped state
		if (m_Scene->GetRuntimeState() == RuntimeState::STOPPED)
		{
			m_CameraController->SetMouseControllesActive(true);
		}
		else
		{
			m_CameraController->SetMouseControllesActive(false);
		}

		auto renderer = Renderer2D::Instance();
		renderer->StartFrame();
		{
			m_Scene->OnUpdate(ts);
		}
		renderer->EndFrame();
	}

	void RuntimeLayer::OnEvent(Event* e)
	{
		EventDispatcher<KeyboardEvent> keyDispatcher;
		keyDispatcher.Dispatch(e, [&](KeyboardEvent* keyEvent) -> bool 
			{
				if (keyEvent->GetKeyCode() == SDL_SCANCODE_P && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
				{
					if (m_Scene->GetRuntimeState() == RuntimeState::STOPPED)
					{
						// Save scene as temp file so it can be restored on Scene Stop
						SceneManager::Instance()->SaveScene("tmp.d2s");
						m_Scene->OnScenePlay();
					}
					else
					{
						m_Scene->OnSceneStop();
						// Restore Scene form temp file
						SceneManager::Instance()->LoadScene("tmp.d2s");
					}

					return true;
				}

				return false;
			});

		if (e->GetEventType() == Stimpi::EventType::WindowEvent)
		{
			auto* we = (Stimpi::WindowEvent*)e;
			if (we->GetType() == Stimpi::WindowEventType::WINDOW_EVENT_RESIZE)
			{
				Renderer2D::Instance()->ResizeCanvas(we->GetWidth(), we->GetHeight());
			}
		}

		// Pass events to the Scene
		m_Scene->OnEvent(e);
	}

}