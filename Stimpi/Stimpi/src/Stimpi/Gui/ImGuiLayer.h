#pragma once

#include <SDL.h>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Window.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class ST_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer(Window* window, SDL_GLContext* glContext);
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void Update(Timestep ts) override;
		void OnEvent(BaseEvent* e) override;
	private:
		ImGuiIO* m_IO{ nullptr };
		WindowSDL* m_Window{ nullptr };
	};
}