#pragma once

#include "Stimpi/Asset/Asset.h"

#include <unordered_map>

namespace Stimpi
{
	using AssetMap = std::unordered_map<AssetHandle, std::shared_ptr<Asset>>;

	class AssetManagerBase
	{
	public:
		virtual std::shared_ptr<Asset> GetAsset(AssetHandle handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandle handle) = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) = 0;
	};
}