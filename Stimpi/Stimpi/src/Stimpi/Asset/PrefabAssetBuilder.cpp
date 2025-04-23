#include "stpch.h"
#include "Stimpi/Asset/PrefabAssetBuilder.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> PrefabAssetBuilder::CreatePrefabAsset(const AssetMetadata& metadata)
	{
		std::shared_ptr<Prefab> prefabAsset = Prefab::Create();
		return std::static_pointer_cast<Asset>(prefabAsset);
	}

}