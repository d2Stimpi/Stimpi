#include "Application.h"

#include <string>

#include "Stimpi/Log.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Engine.h"
#include "Stimpi/Gui/ImGuiLayer.h"

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
		m_LayerStack.AttachOverlay(new ImGuiLayer(m_Window.get(), &m_Context.GetContext()));
	}

	void Application::Run()
	{
		ST_CORE_INFO("Application Run!");

		while (m_Running)
		{
			BaseEvent* event;
			// Read and consume all events
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
						});
				}

				m_LayerStack.OnEvent(event);
			}
			m_LayerStack.Update();
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

	void Application::PopLayer(Layer* layer)
	{
		m_LayerStack.DetachLayer(layer);
	}
}