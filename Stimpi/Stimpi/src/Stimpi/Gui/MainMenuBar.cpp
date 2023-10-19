#include "stpch.h"
#include "Stimpi/Gui/MainManuBar.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

#include <SDL.h>

namespace Stimpi
{
	MainMenuBar::MainMenuBar()
	{

	}

	MainMenuBar::~MainMenuBar()
	{

	}

	void MainMenuBar::OnImGuiRender()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("New Scene"))
				{
					SceneManager::Instance()->NewScene();
				}

				if (ImGui::MenuItem("Load Scene")) 
				{
					SceneManager::Instance()->LoadScene("..\/assets\/scenes\/TestScene_save.d2s");
				}

				if (ImGui::MenuItem("Save Scene", "CTRL+S")) 
				{
					SceneManager::Instance()->SaveScene("..\/assets\/scenes\/TestScene_save.d2s");
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Quit"))
				{
					// TODO: Event queue
					ST_CORE_INFO("Quit triggered from Menu!");
					static SDL_Event event;
					event.window.type = SDL_WINDOWEVENT;
					event.window.event = SDL_WINDOWEVENT_CLOSE;
					event.window.windowID = 1;
					SDL_PushEvent(&event);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}