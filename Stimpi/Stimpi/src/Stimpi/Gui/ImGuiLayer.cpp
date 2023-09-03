#include "ImGuiLayer.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/backend/imgui_impl_sdl2.h"
#include "ImGui/src/backend/imgui_impl_opengl3.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include "Stimpi/Log.h"
#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{

	ImGuiLayer::ImGuiLayer(Window* window, SDL_GLContext* glContext)
	{
		const char* glsl_version = "#version 330";

		ST_CORE_ASSERT(window == nullptr, "Window not set for ImGui Layer!");
		m_Window = (WindowSDL*)window;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
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

		gladLoadGL();
	}

	ImGuiLayer::~ImGuiLayer()
	{
		ST_CORE_TRACE("{0}: ~ImGuiLayer", m_DebugName);
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnAttach()
	{
		ST_CORE_TRACE("{0}: OnAttach", m_DebugName);
	}

	void ImGuiLayer::OnDetach()
	{
		ST_CORE_TRACE("{0}: OnDetach", m_DebugName);
	}

	static bool show_another_window = true;

	void ImGuiLayer::OnEvent(BaseEvent* e)
	{
		ImGui_ImplSDL2_ProcessEvent(e->GetRawSDLEvent());
		//e->LogEvent();

		EventDispatcher<KeyboardEvent> keyDispatcher;
		keyDispatcher.Dispatch(e, [](KeyboardEvent* keyEvent) -> bool {
				if (keyEvent->GetKeyCode() == SDL_SCANCODE_A && keyEvent->GetType() == KeyboardEventType::KEY_EVENT_DOWN)
				{
					show_another_window = !show_another_window;
					return true;
				}
			});
	}

	void ImGuiLayer::Update()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		// TODO: add UI elements here

		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		glViewport(0, 0, (int)m_IO->DisplaySize.x, (int)m_IO->DisplaySize.y);
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

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