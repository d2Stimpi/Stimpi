#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManagerBase.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	using AssetRegistry = std::unordered_map<AssetHandleNew, AssetMetadata>;

	class ST_API AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<AssetNew> GetAsset(AssetHandleNew handle) override;

		bool IsAssetHandleValid(AssetHandleNew handle) override;
		virtual bool IsAssetLoaded(AssetHandleNew handle) override;

		AssetMetadata& GetAssetMetadata(AssetHandleNew handle);
		AssetRegistry& GetAssetRegistry() { 
			int a = 1;
			return m_AssetRegistry; 
		}

		AssetHandleNew RegisterAsset(const AssetMetadata& metadata);
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}