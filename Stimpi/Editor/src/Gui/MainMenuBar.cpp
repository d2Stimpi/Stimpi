#include "stpch.h"
#include "Gui/MainManuBar.h"

#include "Stimpi/Log.h"

#include "Stimpi/Core/Project.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/ShaderImporter.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Utils/PlatformUtils.h"
#include "Stimpi/Utils/FileWatcher.h"
#include "Stimpi/Utils/SystemUtils.h"

#include "Gui/Config/GraphicsConfigPanel.h"
#include "Gui/Config/LayersConfigPanel.h"
#include "Gui/Config/PhysicsConfigPanel.h"

#include "Gui/Panels/PrefabInspectWindow.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

// Window show toggle includes
#include "Gui/Nodes/GraphPanel.h"
#include "Gui/NNode/NGraphPanel.h"
#include "Gui/NNode/NGraphRegistry.h"

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
						//SceneManager::Instance()->NewScene();
						std::string filePath = FileDialogs::SaveFile("d2S Scene (*.d2s)\0*.d2s\0");
						if (!filePath.empty())
						{
							AssetMetadata sceneMetadata = { AssetType::SCENE, filePath };
							auto assetManager = Project::GetEditorAssetManager();
							AssetHandle sceneAssetHandle = assetManager->CreateAsset(sceneMetadata);
							auto scene = AssetManager::GetAsset<Scene>(sceneAssetHandle);

							// Save a new empty scene to make an actual asset file
							FilePath assetFilePath = filePath;
							SceneManager::Instance()->SaveScene(scene.get(), assetFilePath);

							// Register scene
							SceneManager::Instance()->LoadScene(assetFilePath);
						}
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
							// Load NodeGraph registry from assets folder
							auto registryPath = Project::GetGraphsRegistryPath();
							NGraphRegistry::DeserializeGraphRegistry(registryPath);
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
						if (!projectFilePath.empty())
						{
							Project::Save(projectFilePath);
							// Save NodeGraph registry in assets folder
							auto registryPath = Project::GetGraphsRegistryPath();
							NGraphRegistry::SerializeGraphRegistry(registryPath);
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
				// TODO: remove old node graph files
				if (ImGui::MenuItem("Node Panel", nullptr, NGraphPanel::IsVisible()))
				{
					NGraphPanel::ShowWindow(!NGraphPanel::IsVisible());
				}

				if (ImGui::MenuItem("Prefab inspector", nullptr, PrefabInspectWindow::IsVisible()))
				{
					PrefabInspectWindow::ShowWindow(!PrefabInspectWindow::IsVisible());
				}

				ImGui::EndMenu();
			}
#pragma endregion WINDOW

#pragma region TESTING
			if (ImGui::BeginMenu("Testing"))
			{
				if (ImGui::MenuItem("Import Shader"))
				{
					auto shader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/circle.shader");
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