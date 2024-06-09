#include "Gui/ContentBrowserWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Core/Project.h"
#include "Gui/Components/UIPayload.h"

#include "ImGui/src/imgui.h"

#include <functional>

// TODO: move to struct
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

		ReadDirHierarchyData();
	}

	static uint32_t sFileNodeID = 0;
	static uint32_t sFolderNodeID = 0;

	void ContentBrowserWindow::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("Content Browser", &m_Show);
			ImGui::PopStyleVar();
			
			if (ImGui::BeginTable("##ContentBrowserTable", 2, ImGuiTableFlags_Resizable))
			{
				ImGui::TableNextColumn();

				// Child window added so we have separate scroll bar for the first part table column
				if (ImGui::BeginChild(ImGui::GetID("FolderBrowserSection"), ImVec2(0.0f, 0.0f), false))
				{
					if (ImGui::TreeNodeEx("##FileTree", ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, "Assets"))
					{
						if (m_RootFolderNode)
							RecurseNodeDraw(m_RootFolderNode.get());
						ImGui::TreePop();
					}
					ImGui::EndChild();
				}

				ImGui::TableSetColumnIndex(1);
				DrawFolderContentBrowser();
				
				ImGui::EndTable();
			}

			ImGui::End();
		}
	}

	void ContentBrowserWindow::OnUpdate(Timestep ts)
	{
		static float reloadTimer = 0.0f;
		
		reloadTimer += ts.GetMilliseconds();
		if (reloadTimer >= 4000.0f)
		{
			sFileNodeID = 0;	// reset file id "tracker"
			sFolderNodeID = 0;
			// Here refresh assets folder structure
			ResetDirHierarchyData();
			ReadDirHierarchyData();
			reloadTimer = 0.0f;
		}
	}

	void ContentBrowserWindow::OnProjectChanged()
	{
		m_CurrentDirectory = ResourceManager::GetAssetsPath();
		OnCurrentDirChange();
	}

	void ContentBrowserWindow::DrawFolderContentBrowser()
	{
		ImGui::BeginChild(ImGui::GetID("FileBrowserSection"), ImVec2(0.0f, 0.0f), false);
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
			if (relativePath.extension().string() == ".anim")
			{
				UIPayload::BeginSource(PAYLOAD_ANIMATION, path.string().c_str(), path.string().length(), filenameStr.c_str());
			}
		}
		ImGui::EndChild();
	}

	void ContentBrowserWindow::RecurseNodeDraw(FileNode* rootNode)
	{
		ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow /*| ImGuiTreeNodeFlags_OpenOnDoubleClick*/;

		for (auto node : rootNode->m_Children)
		{
			if (node->m_IsDir)
			{
				ImGui::PushID(node->m_ID);
				if (ImGui::TreeNodeEx("", node_flags, "%s", node->m_File.GetFileName().c_str()))
				{

					if (ImGui::IsItemClicked())
					{
						m_CurrentDirectory = node->m_File;
					}

					RecurseNodeDraw(node.get());
					ImGui::TreePop();
				}
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					m_CurrentDirectory = node->m_File;
				}

				ImGui::PopID();
			}
			else
			{
				float startX = ImGui::GetCursorPosX();
				ImGui::PushID(node->m_ID);
				if (ImGui::TreeNodeEx("", leaf_flags, "%s", node->m_File.GetFileName().c_str()))
				{
				}
				//ImGui::SameLine(startX); ImGui::TextDisabled("(?)");
				/*auto asset = AssetManager::GetAssetNoRefCount<Texture>(Project::GetAssestsDir() / "textures\/folder.png");
				Texture* texture = AssetManager::GetAssetData<Texture>(asset);
				ImGui::Image((void*)(intptr_t)texture->GetTextureID(), ImVec2(16, 16));*/
				ImGui::PopID();
			}
		}
	}


	void ContentBrowserWindow::OnCurrentDirChange()
	{
		for (auto asset : m_DirectoryAssets)
		{
			AssetManager::Release(asset);
		}
	}

	void ContentBrowserWindow::ReadDirHierarchyData()
	{
		auto assetsPath = ResourceManager::GetAssetsPath();

		sFolderNodeID++;
		m_RootFolderNode = std::make_shared<FileNode>(assetsPath, sFolderNodeID << 16);
		m_RootFolderNode->m_IsDir = true;

		std::function<void(FilePath, FileNode*)> readDir = [&readDir](FilePath dirPath, FileNode* node) {
			for (auto& directoryEntry : std::filesystem::directory_iterator(dirPath))
			{
				const auto& path = directoryEntry.path();
				auto relativePath = std::filesystem::relative(path, ResourceManager::GetAssetsPath());
				std::string filenameStr = relativePath.filename().string();

				auto newNode = std::make_shared<FileNode>(path);

				if (std::filesystem::is_directory(path))
				{
					sFolderNodeID++;
					newNode->m_ID = sFolderNodeID << 16;
					newNode->m_IsDir = true;
					node->AddChildNode(newNode);
					// read subdirectory
					readDir(path, newNode.get());
				}
				else
				{
					sFileNodeID++;
					newNode->m_ID = sFolderNodeID << 16 + sFileNodeID;
					node->AddChildNode(newNode);
				}
			}
		};

		readDir(assetsPath, m_RootFolderNode.get());
	}

	void ContentBrowserWindow::ResetDirHierarchyData()
	{
		if (m_RootFolderNode)
		{
			std::function<void(FileNode*)> resetNodeData = [&resetNodeData](FileNode* rootNode) {
				for (auto& node : rootNode->m_Children)
				{
					if (!node->m_Children.empty())
					{
						resetNodeData(node.get());
						node->m_Children.clear();
					}
				}
			};

			resetNodeData(m_RootFolderNode.get());
			m_RootFolderNode->m_Children.clear();
		}
	}
}