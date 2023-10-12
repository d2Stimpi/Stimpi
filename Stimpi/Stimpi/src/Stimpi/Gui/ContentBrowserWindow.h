#pragma once

#include <filesystem>

namespace Stimpi
{
	class ContentBrowserWindow
	{
	public:
		ContentBrowserWindow();

		void OnImGuiRender();

	private:
		std::filesystem::path m_CurrentDirectory;
	};
}