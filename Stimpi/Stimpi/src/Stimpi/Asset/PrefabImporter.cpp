#include "stpch.h"
#include "Stimpi/Asset/PrefabImporter.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> PrefabImporter::ImportPrefab(AssetHandle handle, const AssetMetadata& metadata)
	{
		FilePath assetPath = Project::GetAssestsDir() / metadata.m_FilePath.string();
		return std::static_pointer_cast<Asset>(LoadPrefab(assetPath));
	}

	std::shared_ptr<Stimpi::Prefab> PrefabImporter::LoadPrefab(const FilePath& filePath)
	{
		return Prefab::Create(filePath.GetFileName(), filePath);
	}

}