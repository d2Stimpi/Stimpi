#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"

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

		// Graphics config
		GraphicsConfig m_GraphicsConfig;

		// Sorting Layers - Rendering
		std::vector<std::shared_ptr<SortingLayer>> m_SortingLayers;

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
		
		static bool IsProjectPathValid() { return std::filesystem::exists(GetAssestsDir()); }

		static GraphicsConfig GetGraphicsConfig() { return m_ActiveProject.m_GraphicsConfig; }
		static void SetGraphicsConfig(const GraphicsConfig& config) { m_ActiveProject.m_GraphicsConfig = config; }

		static std::vector<std::shared_ptr<SortingLayer>>& GetSortingLayers() { return m_ActiveProject.m_SortingLayers; }
		static std::string GetDefaultSortingLayerName() { return "Default"; }	// TODO: make default layer not removable
	private:
		static ProjectConfig m_ActiveProject;
	};
}