#pragma once

#include "Core.h"
#include <stdio.h>

namespace Stimpi
{

	class STIMPI_API Engine
	{
	public:
		Engine();
		~Engine();

		void StartGameLoop();
	};
}