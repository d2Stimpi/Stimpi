#pragma once

#include "Core.h"
#include <stdio.h>

namespace Stimpi
{

	class ST_API Engine
	{
	public:
		Engine();
		~Engine();

		void StartGameLoop();
	};
}