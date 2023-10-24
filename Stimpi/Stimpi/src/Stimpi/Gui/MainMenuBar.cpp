#include "stpch.h"
#include "Stimpi/Gui/MainManuBar.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "Stimpi/Utils/PlatformUtils.h"

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

				if (ImGui::MenuItem("Open Scene", "Ctrl+O")) 
				{
					std::string filePath = FileDialogs::OpenFile("d2S Scene (*.d2s)\0*.d2s\0");
					if (!filePath.empty())
					{
						SceneManager::Instance()->LoadScene(filePath);
					}
				}

				if (ImGui::MenuItem("Save Scene", "Ctrl+Shift+S")) 
				{
					std::string filePath = FileDialogs::SaveFile("d2S Scene (*.d2s)\0*.d2s\0");
					if (!filePath.empty())
					{
						SceneManager::Instance()->SaveScene(filePath);
					}
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