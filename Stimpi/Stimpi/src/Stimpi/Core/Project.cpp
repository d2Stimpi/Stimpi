#include "stpch.h"
#include "Stimpi/Core/Project.h"

#include "Stimpi/Core/ProjectSerializer.h"
#include "Stimpi/VisualScripting/ExecTreeRegistry.h"

namespace Stimpi
{
	ProjectConfig Project::m_Config;
	std::shared_ptr<AssetManagerBase> Project::m_AssetManager;

	std::filesystem::path Project::GetResourcesSubdir(const Subdir& subdir)
	{
		switch (subdir)
		{
		case Subdir::VisualScripting:	return GetResourcesDir() / "vs";
		case Subdir::Icons:				return GetResourcesDir() / "icons";
		case Subdir::Misc:				return GetResourcesDir() / "misc";
		case Subdir::Shaders:			return GetResourcesDir() / "shaders";
		default:						return GetResourcesDir() / "resources";
		}

		return m_Config.m_ProjectDir / "resources";
	}

	void Project::Save(std::filesystem::path projectFilePath)
	{
		ProjectSerializer::Serialize(projectFilePath, m_Config);

		// Saving AssetRegistry only makes sense in Editor mode
		if (Runtime::GetRuntimeMode() == RuntimeMode::EDITOR)
		{
			std::filesystem::path registry = GetResourcesDir() / GetProjectRegistryName();
			GetEditorAssetManager()->SerializeAssetRegistry(registry);
		}
	}

	void Project::Load(std::filesystem::path projectFilePath)
	{
		m_Config = ProjectSerializer::Deserialize(projectFilePath);

		// Loading AssetRegistry only makes sense in Editor mode
		if (Runtime::GetRuntimeMode() == RuntimeMode::EDITOR)
		{
			std::filesystem::path registry = GetResourcesDir() / GetProjectRegistryName();
			GetEditorAssetManager()->DeserializeAssetRegistry(registry);
		}

		// Load Project specific ExecTrees
		ExecTreeRegistry::LoadProjectExecTrees(GetResourcesSubdir(Subdir::VisualScripting));
	}

	void Project::UpdateDefaultSortingLayerIndex()
	{
		std::string& defaultName = Project::GetDefaultSortingLayerName();

		for (auto& layer : m_Config.m_SortingLayers)
		{
			if (layer->m_Name == defaultName)
			{
				m_Config.m_DefaultLayerIndex = layer->m_LayerIndex;
				break;
			}
		}
	}

	std::shared_ptr<Stimpi::AssetManagerBase> Project::GetAssetManager()
	{
		if (m_AssetManager == nullptr)
		{
			if (Runtime::GetRuntimeMode() == RuntimeMode::EDITOR)
				m_AssetManager.reset(new AssetManagerEditor());
			else // RuntimeMode::APPLICATION
				m_AssetManager.reset(new AssetManagerRuntime());
		}

		return m_AssetManager;
	}

	std::string Project::GetProjectRegistryName()
	{
		std::string registryName = m_Config.m_Name;
		return registryName.append("_Registry.d2ar");
	}

	std::string Project::GetGraphsRegistryPath()
	{
		std::string registryName = m_Config.m_Name;
		registryName.append("_GraphsRegistry.d2ar");
		std::filesystem::path registryPath = GetResourcesDir() / registryName;
		return registryPath.string();
	}

}