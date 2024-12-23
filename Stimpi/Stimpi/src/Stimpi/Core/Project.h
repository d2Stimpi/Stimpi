#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Physics/Physics.h"

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
		// Physics config
		PhysicsConfig m_PhysicsConfig;

		// Sorting Layers - Rendering
		uint32_t m_DefaultLayerIndex = 0;
		std::vector<std::shared_ptr<SortingLayer>> m_SortingLayers;

		ProjectConfig() = default;
	};

	class ST_API Project
	{
	public:
		static std::filesystem::path GetProjectDir() { return m_ActiveProject.m_ProjectDir; }
		static std::filesystem::path GetAssestsDir() { return m_ActiveProject.m_ProjectDir / m_ActiveProject.m_AssestsSubDir; }
		static std::filesystem::path GetScriptsDir() { return m_ActiveProject.m_ProjectDir / m_ActiveProject.m_ScriptsSubDir; }
		static std::filesystem::path GetResourcesDir() { return m_ActiveProject.m_ProjectDir / "resources"; }

		static std::filesystem::path GetStartingScenePath() { return GetAssestsDir() / "scenes" / m_ActiveProject.m_StartingScene; }

		static void Save(std::filesystem::path projectFilePath);
		static void Load(std::filesystem::path projectFilePath);
		
		static bool IsProjectPathValid() { return std::filesystem::exists(GetAssestsDir()); }

		static GraphicsConfig& GetGraphicsConfig() { return m_ActiveProject.m_GraphicsConfig; }
		static void SetGraphicsConfig(const GraphicsConfig& config) { m_ActiveProject.m_GraphicsConfig = config; }

		static std::vector<std::shared_ptr<SortingLayer>>& GetSortingLayers() { return m_ActiveProject.m_SortingLayers; }
		static inline std::string GetDefaultSortingLayerName() { return "Default"; }
		static inline uint32_t GetDefaultSortingLayerIndex() { return m_ActiveProject.m_DefaultLayerIndex; }
		static void UpdateDefaultSortingLayerIndex();
	
		static PhysicsConfig& GetPhysicsConfig() { return m_ActiveProject.m_PhysicsConfig; }
	private:
		static ProjectConfig m_ActiveProject;
	};
}