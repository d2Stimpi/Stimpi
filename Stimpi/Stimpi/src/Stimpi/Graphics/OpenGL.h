#pragma once

#include <SDL.h>

#include "Stimpi/Core/Window.h"

namespace Stimpi
{

	class GLContext
	{
	public:
		~GLContext();

		void CreateContext(Window* window);
		SDL_GLContext& GetContext() { return s_GLcontext; }
	private:
		SDL_GLContext s_GLcontext;
	};
}