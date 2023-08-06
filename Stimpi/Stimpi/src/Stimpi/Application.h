#pragma once

#include "Core.h"

namespace Stimpi
{
	class STIMPI_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// Defined in Client app
	Application* CreateApplication();
}