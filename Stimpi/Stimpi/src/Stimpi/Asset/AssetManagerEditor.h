#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManagerBase.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;

	class ST_API AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) override;
		virtual bool IsAssetLoaded(AssetHandle handle) override;

		AssetMetadata& GetAssetMetadata(AssetHandle handle);
		AssetHandle RegisterAsset(const AssetMetadata& metadata);

		void SerializeAssetRegistry(const FilePath& filePath);
		void DeserializeAssetRegistry(const FilePath& filePath);
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
	};
}