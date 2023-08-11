#include "Window.h"

namespace Stimpi
{
	// Atm here window type is selected
	auto s_WindowImplType = WindowType::SDL;

	Window* Window::CreateAppWindow()
	{
		switch (s_WindowImplType)
		{
			case Stimpi::WindowType::None: { ST_CORE_WARN("WindowImpl: not supported"); return nullptr; }
			case Stimpi::WindowType::SDL: { return new WindowSDL(ST_WINDOW_WIDTH, ST_WINDOW_HEIGHT); }
		}
		ST_CORE_ASSERT(true, "WindowImpl unknown!");
		return nullptr;
	}

	/* SDL Window */

	WindowSDL::WindowSDL(uint32_t w, uint32_t h) 
		: Window(w, h)
	{
		Init();
	}

	WindowSDL::~WindowSDL()
	{
		Deinit();
	}

	void WindowSDL::Init()
	{
		// Init SDL
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
		{
			ST_CORE_ASSERT(true, "Failed to init SDL Error: {0}", SDL_GetError());
		}

		const char* glsl_version = "#version 330";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		m_Window = SDL_CreateWindow("Stimpi Editor v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, window_flags);

		m_ID = SDL_GetWindowID(m_Window);
	}

	void WindowSDL::Deinit()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	bool WindowSDL::PollEvent(Event* e)
	{
		SDL_Event event;
		bool pending = SDL_PollEvent(&event);
		e->SetEvent(event);

		return pending;
	}

	SDL_Window* GetSDLWindow(Window* window)
	{
		if (s_WindowImplType == WindowType::SDL)
		{
			return ((WindowSDL*)window)->GetSDLWindow();
		}
		else
		{
			ST_CORE_CRITICAL("GetSDLWindow: wrong window implementation type!");
			return nullptr;
		}
	}
}