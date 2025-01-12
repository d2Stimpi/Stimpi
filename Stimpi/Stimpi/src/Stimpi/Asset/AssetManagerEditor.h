#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManagerBase.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;
	using AssetLookup = std::unordered_map<std::string, AssetHandle>;

	class ST_API AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;

		bool IsAssetHandleValid(AssetHandle handle) override;
		bool IsAssetLoaded(AssetHandle handle) override;

		AssetMetadata& GetAssetMetadata(AssetHandle handle);
		AssetHandle GetAssetHandle(const FilePath& filePath);
		bool WasAssetUpdated(AssetHandle handle);
		bool ReloadAsset(AssetHandle handle);

		AssetHandle RegisterAsset(const AssetMetadata& metadata);
		bool IsAssetRegistered(const FilePath& filePath);

		void SerializeAssetRegistry(const FilePath& filePath);
		void DeserializeAssetRegistry(const FilePath& filePath);

		// Stats
		size_t GetLoadedAssetsCount();
	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
		// For quick lookup by file path if asset is registered;
		AssetLookup m_AssetLookup;
	};
}