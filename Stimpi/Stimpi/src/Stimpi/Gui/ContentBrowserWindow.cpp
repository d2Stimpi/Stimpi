#include "stpch.h"
#include "Stimpi/Gui/ContentBrowserWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Gui/Components/UIPayload.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	static const std::filesystem::path s_AssetsPath = "../assets";

	ContentBrowserWindow::ContentBrowserWindow()
		: m_CurrentDirectory(s_AssetsPath)
	{

	}

	void ContentBrowserWindow::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetsPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetsPath);
			std::string filenameStr = relativePath.filename().string();
			if (directoryEntry.is_directory())
			{
				if (ImGui::Button(filenameStr.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			else
			{
				if (ImGui::Button(filenameStr.c_str()))
				{

				}
				if (relativePath.extension().string() == ".jpg" || relativePath.extension().string() == ".JPG")
				{
					UIPayload::BeginSource(PAYLOAD_TEXTURE, path.string().c_str(), path.string().length(), filenameStr.c_str());
				}
				if (relativePath.extension().string() == ".d2s")
				{
					UIPayload::BeginSource(PAYLOAD_SCENE, path.string().c_str(), path.string().length(), filenameStr.c_str());
				}
			}
		}

		ImGui::End();
	}

}