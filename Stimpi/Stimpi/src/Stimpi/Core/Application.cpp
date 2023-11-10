#include "stpch.h"
#include "Application.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Layer.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/Time.h"

namespace Stimpi
{
	Application::Application()
	{
		Init();
	}

	Application::~Application()
	{

	}

	void Application::Init()
	{
		Stimpi::Log::Init();
		m_Window.reset(Window::CreateAppWindow());
		m_Context.CreateContext(m_Window.get());
		//m_LayerStack.AttachOverlay(new EditorLayer(m_Window.get(), &m_Context.GetContext()));
	}

	void Application::Run()
	{
		ST_CORE_INFO("Application Run!");

		while (m_Running)
		{
			// Time measure - Start of frame
			Time::Instance()->FrameBegin();
			Timestep ts = Time::Instance()->DeltaTime();

			// Read and consume all events
			ProcessEvents();

			m_LayerStack.Update(ts);

			//Clear all KeyboardEvents this game loop
			InputManager::Instance()->ClearEvents();
			
			// Time measure - End of frame

			Time::Instance()->FPSCapDelay();
			Time::Instance()->FrameEnd();

			m_Window->SwapWindow();
		};
	}

	void Application::Stop()
	{
		m_Running = false;
		ST_CORE_INFO("Stopping Application!");
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.AttachLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.AttachOverlay(layer);
	}

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.DetachLayer(layer);
	}

	/* Private methods */
	void Application::ProcessEvents()
	{
		Event* event;
		while (m_Window->PollEvent(&event))
		{
			if (event != nullptr)
			{
				EventDispatcher<WindowEvent> dispatcher;
				dispatcher.Dispatch(event, [&](WindowEvent* e) -> bool {
					if (e->GetType() == WindowEventType::WINDOW_EVENT_QUIT)
					{
						Stop();
						return true;
					}
					if (e->GetType() == WindowEventType::WINDOW_EVENT_RESIZE)
					{
						m_Window->OnResizeEvent(e->GetWidth(), e->GetHeight());
					}
					return false;
				});

				EventDispatcher<KeyboardEvent> keyboardDispatcher;
				keyboardDispatcher.Dispatch(event, [&](KeyboardEvent* e) -> bool {
					InputManager::Instance()->AddEvent(*e);
					return InputManager::Instance()->HandleKeyboardEvent(*e);
				});

				EventDispatcher<MouseEvent> mouseDispatcher;
				mouseDispatcher.Dispatch(event, [&](MouseEvent* e) -> bool {
					InputManager::Instance()->AddEvent(*e);
					return InputManager::Instance()->HandleMouseEvent(*e);
				});

				m_LayerStack.OnEvent(event);
			}
		}
	}
}