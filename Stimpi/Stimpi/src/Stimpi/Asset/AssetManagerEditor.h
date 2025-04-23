#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManagerBase.h"

namespace Stimpi
{
	class Entity;	// Temp

	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;
	using AssetLookup = std::unordered_map<std::string, AssetHandle>;

	class ST_API AssetReloadHandler
	{
		using OnAssetReloadFunction = std::function<void(std::shared_ptr<Asset>)>;

	public:
		AssetReloadHandler(OnAssetReloadFunction onAssetReload)
			: m_OnAssetReload(onAssetReload)
		{}
		void OnAssetReload(std::shared_ptr<Asset>  asset) { if (m_OnAssetReload) m_OnAssetReload(asset); }

	private:
		OnAssetReloadFunction m_OnAssetReload;
	};

	using AssetReloadHandlers = std::vector<AssetReloadHandler*>;

	/**
	 * AssetManager - Editor version
	 */

	class ST_API AssetManagerEditor : public AssetManagerBase
	{
	public:
		std::shared_ptr<Asset> GetAsset(AssetHandle handle) override;
		AssetHandle GetAssetHandle(const AssetMetadata& metadata) override;

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

		// Asset creation
		AssetHandle CreateAsset(AssetMetadata& metadata);

		// Event handler functions
		void RegisterAssetReloadHandler(AssetReloadHandler* handler);
		void UnregisterAssetReloadHandler(AssetReloadHandler* handler);

	private:
		void OnAssetRegistered(AssetHandle handle, AssetMetadata metadata);

	private:
		AssetMap m_LoadedAssets;
		AssetRegistry m_AssetRegistry;
		
		// For quick lookup by file path if asset is registered;
		AssetLookup m_AssetLookup;

		// Event handlers - callbacks
		AssetReloadHandlers m_AssetReloadHandlers;
	};
}