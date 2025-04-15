#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class AssetManagerRuntime : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;
		AssetHandle GetAssetHandle(const AssetMetadata& metadata) override;
		bool IsAssetHandleValid(AssetHandle handle) override;
		virtual bool IsAssetLoaded(AssetHandle handle) override;
	};
}