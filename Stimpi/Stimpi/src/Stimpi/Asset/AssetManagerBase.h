#pragma once

#include "Stimpi/Asset/AssetNew.h"

namespace Stimpi
{
	using AssetMap = std::unordered_map<AssetHandleNew, std::shared_ptr<AssetNew>>;

	class AssetManagerBase
	{
	public:
		virtual std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) = 0;

		virtual bool IsAssetHandleValid(AssetHandleNew handle) = 0;
		virtual bool IsAssetLoaded(AssetHandleNew handle) = 0;
	};
}