#include "Application.h"


#include "Log.h"

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
		ST_CORE_TRACE("Application Run!");

		//while (1) {};
	}
}