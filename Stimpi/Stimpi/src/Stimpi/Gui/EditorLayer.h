#pragma once

#include <SDL.h>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Window.h"

#include "Stimpi/Scene/Scene.h"

// Gui includes
#include "Stimpi/Gui/SceneConfigWindow.h"
#include "Stimpi/Gui/MainManuBar.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class ST_API EditorLayer : public Layer
	{
	public:
		EditorLayer(Window* window, SDL_GLContext* glContext);
		~EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void Update(Timestep ts) override;
		void OnEvent(Event* e) override;
	private:
		ImGuiIO* m_IO{ nullptr };
		WindowSDL* m_Window{ nullptr };

		//Menus
		MainMenuBar m_MainMenuBar;
		//Windows
		SceneConfigWindow m_SceneConfigWindow;

		//Scene data
		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<Shader> m_ShaderChecker;
		std::shared_ptr<OrthoCamera> m_SceneCamera;
	};
}