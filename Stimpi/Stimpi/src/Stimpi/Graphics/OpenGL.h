#pragma once

#include <SDL.h>

#include "Stimpi/Core/Window.h"
#include "Stimpi/Graphics/Graphics.h"


namespace Stimpi
{

	class ST_API GLContext
	{
	public:
		~GLContext();

		void CreateContext(Window* window);
		SDL_GLContext& GetContext() { return s_GLcontext; }
	private:
		SDL_GLContext s_GLcontext;
	};
}