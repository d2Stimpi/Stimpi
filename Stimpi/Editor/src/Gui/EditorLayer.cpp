#include "stpch.h"
#include "Gui/EditorLayer.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/backend/imgui_impl_sdl2.h"
#include "ImGui/src/backend/imgui_impl_opengl3.h"

#include "Stimpi/Core/Time.h"
#include "Stimpi/Log.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Gui/EditorUtils.h"
#include "Gui/Gizmo2D.h"
#include "Gui/Utils/EditorResources.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <SDL.h>
#include <SDL_opengl.h>

namespace Stimpi
{

	EditorLayer::EditorLayer(Window* window, SDL_GLContext* glContext)
	{
		const char* glsl_version = "#version 330";

		ST_CORE_ASSERT_MSG(window == nullptr, "Window not set for ImGui Layer!");
		m_Window = (WindowSDL*)window;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Game pad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		m_IO = &io;

		// Setup Dear ImGui style
		// Custom style
		SetDarkThemeStyle();	

		// When view ports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(m_Window->GetSDLWindow(), glContext);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Other (Non ImGui) internal data init
		EditorResources::LoadTextues();

		// Scene Init
		m_ShaderChecker.reset(Shader::CreateShader("..\/assets\/shaders\/checkerboard.shader"));
		// Editor camera init
		m_EditorCamera = std::make_shared<Camera>(0.0f, 128.0f, 0.0f, 72.0f); // TODO: expose to Editor
		m_EditorCamera->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_CameraController = std::make_shared<CameraController>(m_EditorCamera.get());

		m_BackgroundCamera = std::make_shared<Camera>(0.0f, 1280.0f, 0.0f, 720.0f);
		m_BackgroundCamera->SetPosition({ 0.0f, 0.0f, 0.0f });

		SceneManager::Instance()->LoadStartingScene();
		OnSceneChangedListener onScneeChanged = [&]() {
			ST_CORE_INFO("EditorLayer - onScneeChanged()");
			
			Renderer2D::Instance()->ClearScene();

			m_Scene = SceneManager::Instance()->GetActiveSceneRef();
			m_Scene->SetCamera(m_EditorCamera.get());

			// Update when scene changes
			SetupComponentContext(m_Scene.get());
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);

		m_Scene = SceneManager::Instance()->GetActiveSceneRef();
		m_Scene->SetCamera(m_EditorCamera.get());

		// Project listener
		OnProjectChangedListener onProjectChanged = [&]() {
			ST_CORE_INFO("EditorLayer - onProjectChanged()");

			m_ContentBrowserWindow.OnProjectChanged();
		};
		ResourceManager::Instance()->RegisterOnProjectChangedListener(onProjectChanged);


		// Window/Panel context set
		SetupComponentContext(m_Scene.get());
	}

	EditorLayer::~EditorLayer()
	{
		ST_CORE_TRACE("{0}: ~EditorLayer", m_DebugName);
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void EditorLayer::OnAttach()
	{
		ST_CORE_TRACE("{0}: OnAttach", m_DebugName);
		Renderer2D::Instance()->EnableLocalRendering(false);
	}

	void EditorLayer::OnDetach()
	{
		ST_CORE_TRACE("{0}: OnDetach", m_DebugName);
	}

	static bool show_demo_window = true;
	static bool show_scene_config_window = true;
	static ImVec2 wsLog;
	static ImVec2 uvLog;

	void EditorLayer::OnEvent(Event* e)
	{
		//ST_CORE_INFO("OnEvent - ImGuiLayer");
		ImGui_ImplSDL2_ProcessEvent(e->GetRawSDLEvent());

		EventDispatcher<KeyboardEvent> keyDispatcher;
		keyDispatcher.Dispatch(e, [](KeyboardEvent* keyEvent) -> bool {
			if (!EditorUtils::WantCaptureKeyboard())	// Bail out if we are processing some input text for example
				return false;

			if (keyEvent->GetKeyCode() == SDL_SCANCODE_P && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
			{
				show_scene_config_window = !show_scene_config_window;
				return true;
			}
			if (keyEvent->GetKeyCode() == SDL_SCANCODE_O && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
			{
				show_demo_window = !show_demo_window;
				return true;
			}
			return false;
		});

		if (e->GetEventType() == Stimpi::EventType::WindowEvent)
		{
			auto* we = (Stimpi::WindowEvent*)e;
			if (we->GetType() == Stimpi::WindowEventType::WINDOW_EVENT_RESIZE)
			{
				//ST_CORE_INFO("ImGui window size: {0}, {1}", wsLog.x, wsLog.y);
				//ST_CORE_INFO("ImGui texture uv: {0}, {1}", uvLog.x, uvLog.y);
			}
		}

		// Pass events to the Scene
		m_Scene->OnEvent(e);
	}

	void EditorLayer::SetDarkThemeStyle()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Text
		colors[ImGuiCol_Text] = ImVec4{ 0.885f, 0.885f, 0.885f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.14f, 0.145f, 0.14f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.24f, 0.245f, 0.24f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// FrameBG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	void EditorLayer::SetupComponentContext(Scene* scene)
	{
		m_SpriteAnimPanel.SetContext(scene);
	}

	void EditorLayer::Update(Timestep ts)
	{
		ImVec4 clear_color = ImVec4(0.0f, 0.55f, 0.60f, 1.00f);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


		// FBO View sample begin
		static ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
		static bool closeWidget = true;

		/* Gizmo2D setup */
		Gizmo2D::BeginFrame();

		/* Main Menu */
		m_MainMenuBar.OnImGuiRender();
		/* Runtime controls */
		m_PlayPanel.OnImGuiRender();
		/* Content Browser */
		m_ContentBrowserWindow.OnUpdate(ts);
		m_ContentBrowserWindow.OnImGuiRender();
		/* Scene Hierarchy */
		m_SceneHierarchyWindow.OnImGuiRender();
		/* Sprite & Animation */
		m_SpriteAnimPanel.OnUpdate(ts);
		m_SpriteAnimPanel.OnImGuiRender();
		/* Global Stats and Configurations */
		m_EditorConfigWindow.OnImGuiRender();
		m_SceneConfigWindow.OnImGuiRender();
		/* Node Panel */
		m_NodePanelNew.OnImGuiRender();

		// Camera movement update - only when in Stopped state
		if (m_Scene->GetRuntimeState() == RuntimeState::STOPPED)
		{
			m_CameraController->SetMouseControllesActive(m_SceneViewWindow.IsHovered());

			if (m_SceneViewWindow.IsFocused())
				m_CameraController->Update(ts);
		}


		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		

		// Custom Rendering stuff
		auto canvasWidth = Stimpi::Renderer2D::Instance()->GetCanvasWidth();
		auto canvasHeight = Stimpi::Renderer2D::Instance()->GetCanvasHeight();

		m_BackgroundCamera->Resize(0.0f, canvasWidth, 0.0f, canvasHeight);
		m_ShaderChecker->SetUniform("u_Projection", m_BackgroundCamera->GetProjectionMatrix());
		m_ShaderChecker->SetUniform("u_resolution", glm::vec2(canvasWidth, canvasHeight));

		// Render Checker Background for editor
		if (m_SceneConfigWindow.ShowCheckerboardBg())
		{
			Stimpi::Renderer2D::Instance()->BeginScene(m_BackgroundCamera->GetOrthoCamera());
			Stimpi::Renderer2D::Instance()->Submit({ canvasWidth / 2.0f, canvasHeight / 2.0f, 0.0f }, { canvasWidth, canvasHeight }, 0.0f, m_ShaderChecker.get());
			Stimpi::Renderer2D::Instance()->EndScene();
		}

		m_Scene->OnUpdate(ts);

		/* Scene View - render after scene so Gizmo won't lag behind 1 frame */
		m_SceneViewWindow.OnImGuiRender();

		auto renderer = Renderer2D::Instance();
		renderer->StartFrame();
		renderer->DrawFrame();
		renderer->EndFrame();

		// GUI Rendering
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
		if (m_IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}
		// Moved to Window class
		//SDL_GL_SwapWindow(m_Window->GetSDLWindow());
	}
}