#include "Application.h"

#include <string>

#include "Stimpi/Log.h"
#include "Stimpi/Core/Layer.h"

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
	}

	void Application::Run()
	{
		ST_CORE_INFO("Application Run!");


		//while (1)
		//{
			m_LayerStack.Update();
			m_LayerStack.Render();
		//};
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