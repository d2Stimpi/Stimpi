#pragma once

#include <SDL.h>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Window.h"

#include "Stimpi/Scene/Scene.h"

// Gui includes
#include "Stimpi/Gui/ContentBrowserWindow.h"
#include "Stimpi/Gui/SceneViewWindow.h"
#include "Stimpi/Gui/SceneHierarchyWindow.h"
#include "Stimpi/Gui/MainManuBar.h"
#include "Stimpi/Gui/SpriteAnimPanel.h"

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
		void SetDarkThemeStyle();

		void SetupComponentContext(Scene* scene);
	private:
		ImGuiIO* m_IO{ nullptr };
		WindowSDL* m_Window{ nullptr };

		//Menus
		MainMenuBar m_MainMenuBar;
		//Windows/Panes same thing xD
		SceneViewWindow m_SceneViewWindow;
		SceneHierarchyWindow m_SceneHierarchyWindow;
		ContentBrowserWindow m_ContentBrowserWindow;
		
		SpriteAnimPanel m_SpriteAnimPanel;

		//Scene data
		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<Camera> m_SceneCamera;
		std::shared_ptr<CameraController> m_CameraController;

		std::shared_ptr<Camera> m_BackgroundCamera;
		std::shared_ptr<Shader> m_ShaderChecker;
	};
}