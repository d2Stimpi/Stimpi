#include "stpch.h"
#include "Stimpi/Asset/PrefabAssetBuilder.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> PrefabAssetBuilder::CreatePrefabAsset(const AssetMetadata& metadata)
	{
		std::string assetFileName = metadata.m_FilePath.GetFileName();
		std::shared_ptr<Prefab> prefabAsset = Prefab::Create(assetFileName);
		return std::static_pointer_cast<Asset>(prefabAsset);
	}

}