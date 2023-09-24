#pragma once

#include <memory>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Window.h"
#include "Stimpi/Graphics/OpenGL.h"

namespace Stimpi
{
	class ST_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Init();
		void Run();
		void Stop();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		void ProcessEvents();
	private:
		LayerStack m_LayerStack;
		bool m_Running{ true };
		std::unique_ptr<Window> m_Window;
		GLContext m_Context;

	};

	// Defined in Client app
	Application* CreateApplication();
}