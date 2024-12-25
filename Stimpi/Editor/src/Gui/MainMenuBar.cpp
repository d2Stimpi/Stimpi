#include "stpch.h"
#include "Gui/MainManuBar.h"

#include "Stimpi/Log.h"

#include "Stimpi/Core/Project.h"

#include "Stimpi/Asset/AssetManager.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Utils/PlatformUtils.h"
#include "Stimpi/Utils/FileWatcher.h"
#include "Stimpi/Utils/SystemUtils.h"

#include "Gui/Config/GraphicsConfigPanel.h"
#include "Gui/Config/LayersConfigPanel.h"
#include "Gui/Config/PhysicsConfigPanel.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

// Window show toggle includes
#include "Gui/Nodes/GraphPanel.h"

#include <SDL.h>

namespace Stimpi
{
	static bool s_ShowDemo = false;

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
#pragma region FILE
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::BeginMenu("New        ##File"))
				{
					if (ImGui::MenuItem("Scene...##FileNew"))
					{
						SceneManager::Instance()->NewScene();
					}

					if (ImGui::MenuItem("Project...##FileNew"))
					{
						
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Open##File"))
				{
					if (ImGui::MenuItem("Scene...##FileOpen", "Ctrl+O"))
					{
						std::string filePath = FileDialogs::OpenFile("d2S Scene (*.d2s)\0*.d2s\0");
						if (!filePath.empty())
						{
							SceneManager::Instance()->LoadScene(filePath);
						}
					}

					if (ImGui::MenuItem("Project...##FileOpen"))
					{
						std::string filePath = FileDialogs::OpenFile("d2S Project (*.d2sproj)\0*.d2sproj\0");
						if (!filePath.empty())
						{
							Project::Load(filePath);
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Save##File"))
				{
					if (ImGui::MenuItem("Scene...##FileSave", "Ctrl+Shift+S"))
					{
						std::string filePath = FileDialogs::SaveFile("d2S Scene (*.d2s)\0*.d2s\0");
						if (!filePath.empty())
						{
							SceneManager::Instance()->SaveScene(filePath);
						}
					}

					if (ImGui::MenuItem("Project...##FileSave"))
					{
						std::string projectFilePath = FileDialogs::SaveFile("d2S Project (*.d2sproj)\0*.d2sproj\0");
						//projectFilePath.append(".d2sproj");
						if (!projectFilePath.empty())
						{
							Project::Save(projectFilePath);
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Quick Save"))
				{
					FilePath savePath = SceneManager::Instance()->GetActiveScenePath();
					SceneManager::Instance()->SaveScene(savePath);
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
#pragma endregion FILE

#pragma region EDIT
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::BeginMenu("Project##Edit"))
				{
					if (ImGui::MenuItem("Graphics"))
					{
						GraphicsConfigPanel::ShowWindow(!GraphicsConfigPanel::IsVisible());
					}

					if (ImGui::MenuItem("Layers"))
					{
						LayersConfigPanel::ShowWindow(!LayersConfigPanel::IsVisible());
					}

					if (ImGui::MenuItem("Physics"))
					{
						PhysicsConfigPanel::ShowWindow(!PhysicsConfigPanel::IsVisible());
					}

					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
#pragma endregion EDIT

#pragma region WINDOW
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Node Panel", nullptr, GraphPanel::IsVisible()))
				{
					GraphPanel::ShowWindow(!GraphPanel::IsVisible());
				}

				ImGui::EndMenu();
			}
#pragma endregion WINDOW

#pragma region TESTING
			if (ImGui::BeginMenu("Testing"))
			{
				if (ImGui::MenuItem("AssetManager"))
				{
					static bool registerHandle = false;
					static AssetHandle handle = 0;
					if (!registerHandle)
					{
						registerHandle = true;
						auto& assetManager = Project::GetEditorAssetManager();
						handle = assetManager->RegisterAsset({ AssetType::TEXTURE, "TestAssetPath\\Sample.png" });
					}

					auto texture = AssetManager::GetAsset<Asset>(handle);
				}

				if (ImGui::MenuItem("Run system cmd"))
				{
					System::ExecuteCmd("mono\\bin\\mcs TempGenClass.cs -r:D:\\GitHub\\Stimpi\\resources\\scripts\\Stimpi-ScriptCore.dll  -target:library  -out:pera2.dll");
				}

				if (ImGui::MenuItem("Test Custom Script"))
				{
					ScriptEngine::LoadCustomClassesFromCoreAssembly({ 
						{ "Stimpi", "ClassCompiler" }
					});
					auto scriptClass = ScriptEngine::GetClassByClassIdentifier({ "Stimpi", "ClassCompiler" });
					if (scriptClass)
					{
						auto scriptInstance = ScriptInstance(scriptClass);
						scriptInstance.InvokeMethod("TestBuild");
					}
					else
					{
						ST_INFO("Class not found!");
					}
				}

				if (ImGui::MenuItem("Field Attributes Test"))
				{
					auto scene = SceneManager::Instance()->GetActiveScene();
					/*std::shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetScriptInstance(scene->FindentityByName("Nero"));
					auto& fields = scriptInstance->GetFields();
					for (auto& f : fields)
					{
						auto& field = f.second;
						ST_CORE_INFO("Field {}, serializable: {}", field->GetName(), field->IsSerializable());
					}*/

					ScriptEngine::LoadCustomClassesFromCoreAssembly({
						{ "Stimpi", "AttributeLookup" }
						});
					auto scriptClass = ScriptEngine::GetClassByClassIdentifier({ "Stimpi", "AttributeLookup" });
					if (scriptClass)
					{
						auto scriptInstance = ScriptInstance(scriptClass);
						auto type = ScriptEngine::GetMonoReflectionTypeByName("Demo.DemoPlayer");
						if (type)
						{
							void* params[2];
							uint32_t value;
							params[0] = type;
							params[1] = &value;
							void* res = scriptInstance.InvokeMethod("Test_GetScriptOrderAttributeValue", 2, params);
							if (res)
								ST_INFO("DemoPlayer {} ScriptOrder defined with value {}", *(bool*)res ? "has" : "doesn't have", value);
						}
					}
					else
					{
						ST_INFO("Class not found!");
					}


					/*ScriptEngine::LoadCustomClassesFromCoreAssembly({
						{ "Stimpi", "AttributeLookup" }
					});
					auto scriptClass = ScriptEngine::GetClassByClassIdentifier({ "Stimpi", "AttributeLookup" });
					if (scriptClass)
					{
						auto scriptInstance = ScriptInstance(scriptClass);
						auto type = ScriptEngine::GetMonoReflectionTypeByName("Sandbox.Player");
						if (type)
						{
							void* attrParam[2];
							attrParam[0] = type;
							attrParam[1] = ScriptEngine::CreateMonoString("camera");
							void* res = scriptInstance.InvokeMethod("HasSerializeFieldAttribute", 2, attrParam);
							if (res)
								ST_INFO("camera {} serializable", *(bool*)res ? "is" : "is not");


							attrParam[1] = ScriptEngine::CreateMonoString("m_AnimComponent123");
							res = scriptInstance.InvokeMethod("HasSerializeFieldAttribute", 2, attrParam);
							if (res)
								ST_INFO("m_AnimComponent {} serializable", *(bool*)res ? "is" : "is not");
						}
						else
						{
							ST_INFO("Type not found!");
						}
					}
					else
					{
						ST_INFO("Class not found!");
					}*/
				}

				ImGui::EndMenu();
			}
#pragma endregion TESTING

			if (ImGui::BeginMenu("ImGui"))
			{
				if (ImGui::MenuItem("ShowDemo", nullptr, s_ShowDemo))
				{
					s_ShowDemo = !s_ShowDemo;
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

		if (s_ShowDemo)
			ImGui::ShowDemoWindow(&s_ShowDemo);
	}
}