#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Runtime.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Physics/Physics.h"
#include "Stimpi/Asset/AssetManagerBase.h"
#include "Stimpi/Asset/AssetManagerEditor.h"
#include "Stimpi/Asset/AssetManagerRuntime.h"

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
		static std::filesystem::path GetProjectDir() { return m_Config.m_ProjectDir; }
		static std::filesystem::path GetAssestsDir() { return m_Config.m_ProjectDir / m_Config.m_AssestsSubDir; }
		static std::filesystem::path GetScriptsDir() { return m_Config.m_ProjectDir / m_Config.m_ScriptsSubDir; }
		static std::filesystem::path GetResourcesDir() { return m_Config.m_ProjectDir / "resources"; }

		static std::filesystem::path GetStartingScenePath() { return GetAssestsDir() / "scenes" / m_Config.m_StartingScene; }

		static void Save(std::filesystem::path projectFilePath);
		static void Load(std::filesystem::path projectFilePath);
		
		static bool IsProjectPathValid() { return std::filesystem::exists(GetAssestsDir()); }

		static GraphicsConfig& GetGraphicsConfig() { return m_Config.m_GraphicsConfig; }
		static void SetGraphicsConfig(const GraphicsConfig& config) { m_Config.m_GraphicsConfig = config; }

		static std::vector<std::shared_ptr<SortingLayer>>& GetSortingLayers() { return m_Config.m_SortingLayers; }
		static inline std::string GetDefaultSortingLayerName() { return "Default"; }
		static inline uint32_t GetDefaultSortingLayerIndex() { return m_Config.m_DefaultLayerIndex; }
		static void UpdateDefaultSortingLayerIndex();
	
		static PhysicsConfig& GetPhysicsConfig() { return m_Config.m_PhysicsConfig; }

		static std::shared_ptr<AssetManagerBase> GetAssetManager();
		static std::shared_ptr<AssetManagerEditor> GetEditorAssetManager() { return std::static_pointer_cast<AssetManagerEditor>(m_AssetManager); }
		static std::shared_ptr<AssetManagerRuntime> GetRuntimeAssetManager() { return std::static_pointer_cast<AssetManagerRuntime>(m_AssetManager); }
	private:
		static ProjectConfig m_Config;

		static std::shared_ptr<AssetManagerBase> m_AssetManager;
	};
}