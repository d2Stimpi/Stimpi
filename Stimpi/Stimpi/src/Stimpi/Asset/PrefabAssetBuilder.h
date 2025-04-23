#pragma once

#include "Stimpi/Asset/Prefab.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class PrefabAssetBuilder
	{
	public:
		static std::shared_ptr<Asset> CreatePrefabAsset(const AssetMetadata& metadata);
	};
}