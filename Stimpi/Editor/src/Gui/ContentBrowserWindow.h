#pragma once

#include "Gui/Components/Thumbnail.h"
#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Asset/Asset.h"

#include <filesystem>

namespace Stimpi
{
	struct FileNode
	{
		FilePath m_File;
		
		// ImGui data
		bool m_IsDir = false;
		uint32_t m_ID = 0; // ID for UI Tree node
		std::vector<std::shared_ptr<FileNode>> m_Children;
		
		// AssetData
		AssetHandle m_Handle;

		FileNode() = default;
		FileNode(const FileNode&) = default;
		FileNode(const FilePath& name)
			: m_File(name), m_IsDir(false), m_ID(0)
		{}
		FileNode(const FilePath& name, uint32_t id)
			: m_File(name), m_IsDir(false), m_ID(id)
		{}

		void AddChildNode(std::shared_ptr<FileNode> node)
		{
			m_Children.emplace_back(node);
		}
	};

	class ContentBrowserWindow
	{
	public:
		ContentBrowserWindow();

		void OnImGuiRender();
		void OnUpdate(Timestep ts);
		void OnProjectChanged();

	private:
		void DrawFolderContentBrowser();
		void OnCurrentDirChange();
		void RecurseNodeDraw(FileNode* rootNode);
		void ReadDirHierarchyData();
		void ResetDirHierarchyData();
		void RefreshDirHierachyData();
		void ThumbnailPopup();

	private:
		bool m_Show = true;
		std::filesystem::path m_CurrentDirectory;

		std::shared_ptr<FileNode> m_RootFolderNode;
	};
}