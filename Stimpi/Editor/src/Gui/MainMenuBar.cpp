#include "stpch.h"
#include "Gui/MainManuBar.h"

#include "Stimpi/Log.h"

#include "Stimpi/Core/Project.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Utils/PlatformUtils.h"
#include "Stimpi/Utils/FileWatcher.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

// Window show toggle includes
#include "Gui/Nodes/NodePanel.h"
#include "Gui/Nodes/GraphPanel.h"

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
			if (ImGui::BeginMenu("File"))
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
			// File - end

			if (ImGui::BeginMenu("Project"))
			{
				if (ImGui::MenuItem("Open"))
				{
					//std::string filePath = FileDialogs::OpenFolder();
					std::string filePath = FileDialogs::OpenFile("d2S Project (*.d2sproj)\0*.d2sproj\0");
					if (!filePath.empty())
					{
						Project::Load(filePath);
					}
				}

				if (ImGui::MenuItem("Save"))
				{
					std::string projectFilePath = FileDialogs::SaveFile("d2S Project (*.d2sproj)\0*.d2sproj\0");
					projectFilePath.append(".d2sproj");
					if (!projectFilePath.empty())
					{
						Project::Save(projectFilePath);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Node Panel", nullptr, NodePanel::IsVisible()))
				{
					NodePanel::ShowWindow(!NodePanel::IsVisible());
				}

				if (ImGui::MenuItem("Node Panel - v2", nullptr, GraphPanel::IsVisible()))
				{
					GraphPanel::ShowWindow(!GraphPanel::IsVisible());
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Testing"))
			{
				if (ImGui::MenuItem("Test Custom Script"))
				{
					ScriptEngine::LoadCustomClassesFromCoreAssembly({ 
						{ "Stimpi", "Compiler" } 
					});
					auto scriptClass = ScriptEngine::GetClassByName("Stimpi.Compiler");
					if (scriptClass)
					{
						auto scriptInstance = ScriptInstance(scriptClass);
						scriptInstance.InvokeMethod("DoSomeStuff");
					}
					else
					{
						ST_INFO("Class Stimpi.CompileCodeTest not found!");
					}
				}

				ImGui::EndMenu();
			}

#if 0
			if (ImGui::BeginMenu("Mono"))
			{
				if (ImGui::MenuItem("Reload"))
				{
					ScriptEngine::ReloadAssembly();
				}

				ImGui::EndMenu();
			}

			// Temp Test stuff
			if (ImGui::BeginMenu("Test"))
			{
				if (ImGui::MenuItem("Folder Watcher"))
				{
					std::filesystem::path assetsPath = ResourceManager::GetProjectPath();
					FileWatcher::AddWatcher(std::filesystem::absolute(assetsPath), FileWatchListener([](SystemShellEvent* e) {
							ST_CORE_INFO("Watcher - received event: [{}] {} - {}", (int)e->GetEventType(), e->GetFilePath(), e->GetNewFilePath());
						}));
				}

				if (ImGui::MenuItem("File Watcher"))
				{
					std::filesystem::path assetsPath = ResourceManager::GetProjectPath();
					std::string filePathStr = std::filesystem::absolute(assetsPath).string() + "\\file.txt";
					std::filesystem::path filePath = filePathStr;
					FileWatcher::AddWatcher(std::filesystem::absolute(filePath), FileWatchListener([](SystemShellEvent* e) {
							ST_CORE_INFO("Watcher - received event: [{}] {} - {}", (int)e->GetEventType(), e->GetFilePath(), e->GetNewFilePath());
						}));
				}

				if (ImGui::MenuItem("Remove Watcher"))
				{
					std::filesystem::path assetsPath = ResourceManager::GetProjectPath();
					FileWatcher::RemoveWatcher(std::filesystem::absolute(assetsPath));
				}

				ImGui::EndMenu();
			}
#endif
			ImGui::EndMainMenuBar();
		}
	}
}