#pragma once

#include "Core.h"

namespace Stimpi
{
	class ST_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Init();
		void Run();
	};

	// Defined in Client app
	Application* CreateApplication();
}