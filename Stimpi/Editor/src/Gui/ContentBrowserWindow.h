#pragma once

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
		std::filesystem::path m_CurrentDirectory;
	};
}