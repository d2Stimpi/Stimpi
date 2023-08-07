#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"

namespace Stimpi
{
	class ST_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Init();
		void Run();

		void PushLayer(Layer* layer);
		void PopLayer(Layer* layer);
	private:
		LayerStack m_LayerStack;
	};

	// Defined in Client app
	Application* CreateApplication();
}