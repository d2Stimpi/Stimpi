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
		m_LayerStack.AttachOverlay(new ImGuiLayer(m_Window.get(), &WindowSDL::GetGLContext()));
	}

	void Application::Run()
	{
		ST_CORE_INFO("Application Run!");

		while (m_Running)
		{
			// Read and consume all events
			Event event;

			while (m_Window->PollEvent(&event))
			{
				// Check exit events
				if ((event.GetRawEvent().type == SDL_WINDOWEVENT && event.GetRawEvent().window.event == SDL_WINDOWEVENT_CLOSE && event.GetRawEvent().window.windowID == m_Window->GetID())
					|| (event.GetRawEvent().type == SDL_QUIT))
				{
					Stop();
					event.Handled();
				}
				// Pass events to layer stack
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