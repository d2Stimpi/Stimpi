#include "Gui/ContentBrowserWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Core/Project.h"
#include "Gui/Components/UIPayload.h"

#include "ImGui/src/imgui.h"

#define THUMBNAIL_WIDTH 96
#define THUMBNAIL_HEIGHT 116
#define THUMBNAIL_SIZE (ImVec2(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT))

namespace Stimpi
{
	ContentBrowserWindow::ContentBrowserWindow()
		: m_CurrentDirectory(ResourceManager::GetAssetsPath())
	{
		// Trigger load of used icons
		// TODO: isolate to a class that manages UI resources
		AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/folder.png");
		AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/shader.png");
		AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/scene.png");
	}

	void ContentBrowserWindow::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::Begin("Content Browser", &m_Show);

			if (m_CurrentDirectory != std::filesystem::path(ResourceManager::GetAssetsPath()))
			{
				if (ImGui::Button("<-"))
				{
					m_CurrentDirectory = m_CurrentDirectory.parent_path();
					OnCurrentDirChange();
				}
			}

			// Cursor for drawing Thumbnail grid
			ImVec2 cursor = ImGui::GetCursorScreenPos();
			ImVec2 startCursor = ImGui::GetCursorScreenPos();
			ImVec2 winSize = ImGui::GetContentRegionAvail();
			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, ResourceManager::GetAssetsPath());
				std::string filenameStr = relativePath.filename().string();

				ImGuiStyle& style = ImGui::GetStyle();
				ImVec2 itemSpacing = style.ItemSpacing;

				AssetHandle asset;
				ThumbnailType type = GetThumbnailTypeFromFile(path);
				switch (type)
				{
				case ThumbnailType::NONE:
					asset = AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/shader.png");
					break;
				case ThumbnailType::DIRECTORY:
					asset = AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/folder.png");
					break;
				case ThumbnailType::TEXTURE:
					asset = AssetManager::GetAssetNoRefCount<Texture>(path);
					break;
				case ThumbnailType::SHADER:
					asset = AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/shader.png");
					break;
				case ThumbnailType::SCENE:
					asset = AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/scene.png");
					break;
				default:
					break;
				}

				Texture* texture = AssetManager::GetAsset(asset).As<Texture>();
				if (texture->Loaded())
				{
					if (Thumbnail::Button(filenameStr.c_str(), texture, cursor, THUMBNAIL_SIZE))
					{
						if (type == ThumbnailType::DIRECTORY)
						{
							m_CurrentDirectory /= path.filename();
							OnCurrentDirChange();
						}
					}
					// Check when to drop to next line for drawing (2xWidth because ImGui::SameLine moves cursor after the check)
					if (winSize.x > cursor.x - startCursor.x + 2 * THUMBNAIL_WIDTH)
						ImGui::SameLine();
					cursor = ImGui::GetCursorScreenPos();
				}

				// Drag & Drop handling
				if (relativePath.extension().string() == ".jpg" || relativePath.extension().string() == ".JPG" || relativePath.extension().string() == ".png")
				{
					UIPayload::BeginSource(PAYLOAD_TEXTURE, path.string().c_str(), path.string().length(), filenameStr.c_str());
				}
				if (relativePath.extension().string() == ".d2s")
				{
					UIPayload::BeginSource(PAYLOAD_SCENE, path.string().c_str(), path.string().length(), filenameStr.c_str());
				}
			}
			ImGui::End();
		}
	}

	void ContentBrowserWindow::OnProjectChanged()
	{
		m_CurrentDirectory = ResourceManager::GetAssetsPath();
		OnCurrentDirChange();
	}

	void ContentBrowserWindow::OnCurrentDirChange()
	{
		for (auto asset : m_DirectoryAssets)
		{
			AssetManager::Release(asset);
		}
	}

}