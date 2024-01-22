#pragma once

#include "Stimpi/Core/Core.h"

#include <filesystem>

namespace Stimpi
{
	struct ST_API ProjectConfig
	{
		std::string m_Name = "Unnamed";

		std::string m_StartingScene = "Unnamed.d2s";

		std::filesystem::path m_ProjectDir;
		std::filesystem::path m_AssestsSubDir;
		std::filesystem::path m_ScriptsSubDir;

		ProjectConfig() = default;
	};

	class ST_API Project
	{
	public:
		static std::filesystem::path GetProjectDir() { return m_ActiveProject.m_ProjectDir; }
		static std::filesystem::path GetAssestsDir() { return m_ActiveProject.m_ProjectDir / m_ActiveProject.m_AssestsSubDir; }
		static std::filesystem::path GetScriptsDir() { return m_ActiveProject.m_ProjectDir / m_ActiveProject.m_ScriptsSubDir; }

		static std::filesystem::path GetStartingScenePath() { return GetAssestsDir() / "scenes" / m_ActiveProject.m_StartingScene; }

		static void Save(std::filesystem::path projectFilePath);
		static void Load(std::filesystem::path projectFilePath);
		
	private:
		static ProjectConfig m_ActiveProject;
	};
}