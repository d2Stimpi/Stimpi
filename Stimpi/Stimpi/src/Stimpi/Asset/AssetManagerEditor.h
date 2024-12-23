#pragma once

#include "Stimpi/Asset/AssetManagerBase.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	using AssetRegistry = std::unordered_map<AssetHandleNew, AssetMetadata>;

	class AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) override;

		bool IsAssetHandleValid(AssetHandleNew handle) override;
		virtual bool IsAssetLoaded(AssetHandleNew handle) override;

		AssetMetadata& GetAssetMetadata(AssetHandleNew handle);
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}