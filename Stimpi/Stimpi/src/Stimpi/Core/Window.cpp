#include "stpch.h"
#include "Window.h"

#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Utils/FileWatcher.h"
#include "Stimpi/Utils/PlatformUtils.h"
#include "Stimpi/Core/Event.h"

#include <glad/glad.h>
#include <SDL_syswm.h>
#include <CommCtrl.h>

namespace Stimpi
{
	static LRESULT CALLBACK SubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		ShellEvetData event;

		if (uMsg == STSHELL_USER_EVENT)
		{
			ShellUtils::ProcessEvent(wParam, lParam, &event);
			if (event.m_Event != 0)
			{
				ST_CORE_INFO("ShellEvet: {} - {} : {}", event.m_Event, event.m_FilePath, event.m_NewFilePath);
				// TODO: pass event to FileWatcher
				std::shared_ptr<SystemShellEvent> shEvent = std::make_shared<SystemShellEvent>();
				shEvent.reset(SystemShellEvent::CreateSystemShellEvent(event));
				FileWatcher::ProcessShellEvent(shEvent);
			}
		}

		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	// Window type is selected
	auto s_WindowImplType = WindowType::SDL;

	Window* Window::CreateAppWindow()
	{
		switch (s_WindowImplType)
		{
			case Stimpi::WindowType::None: { ST_CORE_WARN("WindowImpl: not supported"); return nullptr; }
			case Stimpi::WindowType::SDL: { return new WindowSDL(ST_WINDOW_WIDTH, ST_WINDOW_HEIGHT); }
		}
		ST_CORE_ASSERT_MSG(true, "WindowImpl unknown!");
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
			ST_CORE_ASSERT_MSG(true, "Failed to init SDL Error: {0}", SDL_GetError());
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
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		m_Window = SDL_CreateWindow("Stimpi Editor v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_Width, m_Height, window_flags);

		m_ID = SDL_GetWindowID(m_Window);

		// Enable processing System events
		uint8_t state = SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

		// Create a sub window class in order to capture ShellEvents
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(m_Window, &wmInfo);
		SetWindowSubclass(wmInfo.info.win.window, &SubClassProc, 1, 0);
	}

	void WindowSDL::Deinit()
	{
		SDL_DestroyWindow(m_Window);
		SDL_Quit();
	}

	bool WindowSDL::PollEvent(Event** e)
	{
		SDL_Event event;
		bool pending = SDL_PollEvent(&event);

		if (pending)
		{
			*e = EventFactory::EventCreate(event);
			if (*e != nullptr)
			{
				(*e)->SetRawSDLEvent(event);
			}
		}

		return pending;
	}

	void WindowSDL::SwapWindow()
	{
		SDL_GL_SwapWindow(m_Window);
	}

	void WindowSDL::OnResizeEvent(uint32_t width, uint32_t height)
	{
		if (Renderer2D::Instance()->IsLocalRendering()) // Do not resize for editor rendering
			Renderer2D::Instance()->ResizeCanvas(width, height);
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