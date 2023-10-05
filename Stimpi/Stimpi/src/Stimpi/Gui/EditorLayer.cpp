#include "Stimpi/Gui/EditorLayer.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/backend/imgui_impl_sdl2.h"
#include "ImGui/src/backend/imgui_impl_opengl3.h"

#include "Stimpi/Core/Time.h"
#include "Stimpi/Log.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Scene/SceneManager.h"

#include <SDL.h>
#include <SDL_opengl.h>

namespace Stimpi
{

	EditorLayer::EditorLayer(Window* window, SDL_GLContext* glContext)
	{
		const char* glsl_version = "#version 330";

		ST_CORE_ASSERT(window == nullptr, "Window not set for ImGui Layer!");
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
		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
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

		// Scene Init
		Stimpi::SceneManager::Instance()->LoadScene("SceneTest.data");
		Stimpi::OnSceneChangedListener onScneeChanged = [&]() {
			ST_CORE_INFO("OpenGLTestLayer - onScneeChanged()");
			//TODO: move Scene stuff to Editor
			m_Scene = Stimpi::SceneManager::Instance()->GetActiveSceneRef();
		};
		Stimpi::SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);
		m_Scene = Stimpi::SceneManager::Instance()->GetActiveSceneRef();
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
				if (keyEvent->GetKeyCode() == SDL_SCANCODE_A && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
				{
					show_scene_config_window = !show_scene_config_window;
					return true;
				}
				if (keyEvent->GetKeyCode() == SDL_SCANCODE_D && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
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
	}

	void EditorLayer::Update(Timestep ts)
	{
		ImVec4 clear_color = ImVec4(0.0f, 0.55f, 0.60f, 1.00f);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


		// FBO Veiw sample begin
		static ImGuiWindowFlags flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
		static bool closeWidget = true;
		static bool use_work_area = false;

		/* Main Menu */
		m_MainMenuBar.Draw();

		//ImGui::SetNextWindowSize(ImVec2(1280, 720), ImGuiCond_Once);
		//ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(1280, 720));

		ImGui::Begin("OpenGL Main Scene View", &closeWidget, flags);

		static bool use_text_color_for_tint = true;
		// Flip the FBO texture img
		ImVec4 tint_col = use_text_color_for_tint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(0.0f, 0.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 ws = ImGui::GetContentRegionAvail();

		int wWidth = ImGui::GetWindowWidth();
		int wHeight = ImGui::GetWindowHeight();
		auto frameBuffer = Renderer2D::Instace()->GetFrameBuffer();

		wsLog = ImVec2(wWidth, wHeight);//ws;
		uvLog = ImVec2(ws.x / frameBuffer->GetWidth(), ws.y / frameBuffer->GetHeight());

		static ImVec2 test = wsLog;

		if ((test.x != wsLog.x) || test.y != wsLog.y)
		{
			ST_CORE_INFO("Resize - Editor window size: {0}, {1}", wsLog.x, wsLog.y);
			ST_CORE_INFO("Resize - Editor texture uv: {0}, {1}", uvLog.x, uvLog.y);
			//Renderer2D::Instace()->ResizeCanvas(ws.x, ws.y);
		}

		test = wsLog;

		ImVec2 uv_min = ImVec2(0.0f, ws.y / frameBuffer->GetHeight());
		ImVec2 uv_max = ImVec2(ws.x / frameBuffer->GetWidth(), 0.0f);
		//ImVec2 uv_min = ImVec2(0.0f, 1.0f);
		//ImVec2 uv_max = ImVec2(1.0f, 0.0f);
		//ImGui::Image((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(frameBuffer->GetWidth(), frameBuffer->GetHeight()), uv_min, uv_max, tint_col, border_col);
		//ImGui::Image((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(wWidth, wHeight), uv_min, uv_max, tint_col, border_col);
		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + ws.x, pos.y + ws.y), uv_min, uv_max);
		ImGui::End();
		// FBO View sample end

		// TODO: move show_scene_config_window to class
		m_SceneConfigWindow.Draw();

		if (show_scene_config_window)
		{
			ImGui::Begin("Scene Config", &show_scene_config_window);
			ImGui::Text("Resolution");
			const char* resolution_items[] = { "1920x1080", "1280x720" };
			static int current_resolution = 1;
			const float combo_width = ImGui::GetWindowWidth() * 0.80f;
			ImGui::SetNextItemWidth(combo_width);
			if (ImGui::Combo("##Resolution", &current_resolution, resolution_items, IM_ARRAYSIZE(resolution_items)))
			{
				ST_CORE_INFO("Combo - Resolution: Secleted index {0}", current_resolution);
				//Resize Scene Canvas
				if (current_resolution == 0) Renderer2D::Instace()->ResizeCanvas(1920, 1080);
				if (current_resolution == 1) Renderer2D::Instace()->ResizeCanvas(1280, 720);
			};
			ImGui::Separator();
			ImGui::Text("Application FPS");
			const char* fps_items[] = { "60fps", "48fps", "24fps" };
			static int current_fps = 0;
			ImGui::SetNextItemWidth(combo_width);
			if (ImGui::Combo("##FPS", &current_fps, fps_items, 3))
			{
				if (current_fps == 0) Time::Instance()->SetFPS(60);
				if (current_fps == 1) Time::Instance()->SetFPS(48);
				if (current_fps == 2) Time::Instance()->SetFPS(24);
			};
			ImGui::Separator();
			// Scene Entities hierarchy view
			ImGui::Text("Scene elements:");


			ImGui::PushID(1);
			if (ImGui::TreeNode("Basic trees", "Object"))
			{

				ImGui::TreePop();
			}
			ImGui::PopID();

			ImGui::End(); //Config window
		}
		
		static bool closeWidget2 = true;
		ImGui::Begin("Game Stats", &closeWidget2);
		ImGui::Text("Application FPS %.1f", Time::Instance()->GetActiveFPS());
		ImGui::Text("Average %.1f ms/frame", 1000.0f / Time::Instance()->GetActiveFPS());
		//if (ImGui::Button("Close Me"))
		//	closeWidget2 = false;
		ImGui::End();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		
		// Rendering
		ImGui::Render();

		// Custom Rendering stuff
		auto renderer = Renderer2D::Instace();
		renderer->StartFrame();
		renderer->DrawFrame();
		renderer->EndFrame();

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
		SDL_GL_SwapWindow(m_Window->GetSDLWindow());
	}
}