#include "stpch.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Core/ProjectSerializer.h"


namespace Stimpi
{
	ProjectConfig Project::m_Config;
	std::shared_ptr<AssetManagerBase> Project::m_AssetManager;

	void Project::Save(std::filesystem::path projectFilePath)
	{
		ProjectSerializer::Serialize(projectFilePath, m_Config);
	}

	void Project::Load(std::filesystem::path projectFilePath)
	{
		m_Config = ProjectSerializer::Deserialize(projectFilePath);
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

}