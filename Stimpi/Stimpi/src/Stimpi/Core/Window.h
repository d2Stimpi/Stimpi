#pragma once

#include <SDL.h>
#include "Stimpi/Log.h"
#include "Stimpi/Core/Event.h"

/* TODO: 
* - Finish configuring of non-ui (editor) rendering.
*/

namespace Stimpi
{
	enum class WindowType{ None = 0, SDL };

	struct WindowConfiguration
	{
		uint32_t m_WindowWidth = 1280;
		uint32_t m_WindowHeight = 720;
	};

	class Window
	{
	public:
		Window(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {};
		virtual ~Window() {};

		virtual void Init() = 0;
		virtual void Deinit() = 0;
		virtual bool PollEvent(Event** e) = 0;

		// Perform Render buffer swapping
		virtual void SwapWindow() = 0;

		// OnEvent Callbacks
		virtual void OnResizeEvent(uint32_t width, uint32_t height) = 0;

		static Window* CreateAppWindow();
		static WindowConfiguration GetWindowConfiguration();

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		uint32_t GetID() { return m_ID; }

	protected:
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ID = 0;
	};


	class WindowSDL : public Window
	{
	public:
		WindowSDL(uint32_t w, uint32_t h);
		~WindowSDL();

		void Init() override;
		void Deinit() override;
		bool PollEvent(Event** e) override;

		// Perform Render buffer swapping
		void SwapWindow() override;

		// OnEvent Callbacks
		void OnResizeEvent(uint32_t width, uint32_t height) override;

		SDL_Window* GetSDLWindow() { return m_Window; }
	private:
		SDL_Window* m_Window;
	};

	SDL_Window* GetSDLWindow(Window* window);
}