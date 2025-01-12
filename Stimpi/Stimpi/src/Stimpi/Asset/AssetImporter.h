#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Asset> ImportAssetAsync(AssetHandle handle, const AssetMetadata& metadata);
	};
}