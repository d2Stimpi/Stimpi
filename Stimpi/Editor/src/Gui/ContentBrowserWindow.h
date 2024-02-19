#pragma once

#include "Gui/Components/Thumbnail.h"
#include "Stimpi/Scene/Assets/AssetManager.h"

#include <filesystem>

namespace Stimpi
{
	class ContentBrowserWindow
	{
	public:
		ContentBrowserWindow();

		void OnImGuiRender();
		void OnProjectChanged();

	private:
		void OnCurrentDirChange();

	private:
		bool m_Show = true;
		std::filesystem::path m_CurrentDirectory;
		std::vector<AssetHandle> m_DirectoryAssets;
	};
}