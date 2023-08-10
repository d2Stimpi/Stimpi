#pragma once

#include <memory>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Window.h"

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
		LayerStack m_LayerStack;
		bool m_Running{ true };
		std::unique_ptr<Window> m_Window;

	};

	// Defined in Client app
	Application* CreateApplication();
}