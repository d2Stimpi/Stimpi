#pragma once

#include <SDL.h>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Layer.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Window.h"

#include "Stimpi/Scene/Scene.h"

// Gui includes
#include "Gui/ContentBrowserWindow.h"
#include "Gui/SceneViewWindow.h"
#include "Gui/Panels/SceneHierarchyWindow.h"
#include "Gui/MainManuBar.h"
#include "Gui/PlayPanel.h"
#include "Gui/SpriteAnimPanel.h"
#include "Gui/Stats/EditorConfig.h"
#include "Gui/Config/SceneConfigPanel.h"
#include "Gui/Config/GraphicsConfigPanel.h"
#include "Gui/Config/LayersConfigPanel.h"
#include "Gui/Config/PhysicsConfigPanel.h"
// Test - WIP
#include "Gui/Nodes/GraphPanel.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class EditorLayer : public Layer
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
		//Runtime controls
		PlayPanel m_PlayPanel;
		//Windows/Panels same thing xD
		SceneViewWindow m_SceneViewWindow;
		SceneHierarchyWindow m_SceneHierarchyWindow;
		ContentBrowserWindow m_ContentBrowserWindow;
		
		SpriteAnimPanel m_SpriteAnimPanel;

		//Global stats/config
		EditorConfig m_EditorConfigWindow;
		SceneConfigPanel m_SceneConfigWindow;

		// Edit/Project/
		GraphicsConfigPanel m_GraphicsConfigPanel;
		LayersConfigPanel m_LayersConfigPanel;
		PhysicsConfigPanel m_PhysicsConfigPanel;

		//Test-WIP
		GraphPanel m_NodePanelNew;

		//Scene data
		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<Camera> m_EditorCamera;
		std::shared_ptr<CameraController> m_CameraController;

		std::shared_ptr<Camera> m_BackgroundCamera;
		std::shared_ptr<Shader> m_ShaderChecker;
	};
}