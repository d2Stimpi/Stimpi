#include "stpch.h"
#include "Stimpi/Asset/PrefabAssetBuilder.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> PrefabAssetBuilder::CreatePrefabAsset(const AssetMetadata& metadata)
	{
		std::string name = metadata.m_FilePath.GetFileName();
		std::shared_ptr<Prefab> prefabAsset = Prefab::Create(name);
		return std::static_pointer_cast<Asset>(prefabAsset);
	}

}