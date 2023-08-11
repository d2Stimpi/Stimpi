#pragma once

#include <SDL.h>
#include "Stimpi/Log.h"
#include "Stimpi/Core/Event.h"

#define ST_WINDOW_WIDTH 1280
#define ST_WINDOW_HEIGHT 720

namespace Stimpi
{
	enum class WindowType{ None = 0, SDL };

	class Window
	{
	public:
		Window(uint32_t w, uint32_t h) : m_Width(w), m_Height(h) {};
		virtual ~Window() {};

		virtual void Init() = 0;
		virtual void Deinit() = 0;
		virtual bool PollEvent(Event* e) = 0;

		static Window* CreateAppWindow();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		uint32_t GetID() { return m_ID; }
	protected:
		uint32_t m_Width{ 0 };
		uint32_t m_Height{ 0 };
		uint32_t m_ID{ 0 };
	};


	class WindowSDL : public Window
	{
	public:
		WindowSDL(uint32_t w, uint32_t h);
		~WindowSDL();

		void Init() override;
		void Deinit() override;
		bool PollEvent(Event* e) override;

		SDL_Window* GetSDLWindow() { return m_Window; }
	private:
		SDL_Window* m_Window;
	};

	SDL_Window* GetSDLWindow(Window* window);
}