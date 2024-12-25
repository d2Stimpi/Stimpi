#include "stpch.h"
#include "Stimpi/Asset/AssetManagerEditor.h"
#include "Stimpi/Asset/AssetImporter.h"

namespace Stimpi
{
	std::shared_ptr<Asset> AssetManagerEditor::GetAsset(AssetHandle handle)
	{
		std::shared_ptr<Asset> asset;

		// 1. Is handle valid?
		if (!IsAssetHandleValid(handle))
			return nullptr;
		
		// 2. Is asset loaded, load it if not.
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			// Load asset or check if loading is finished
			auto& metadata = GetAssetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
			{
				ST_CORE_ERROR("Failed to import asset: {}", metadata.m_FilePath.string());
			}
			else
			{
				m_LoadedAssets[handle] = asset;
			}
		}

		// 3. Return asset.
		return asset;
	}

	bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle)
	{
		return handle != 0 && (m_AssetRegistry.find(handle) != m_AssetRegistry.end());
	}

	bool AssetManagerEditor::IsAssetLoaded(AssetHandle handle)
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	Stimpi::AssetMetadata& AssetManagerEditor::GetAssetMetadata(AssetHandle handle)
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	AssetHandle AssetManagerEditor::RegisterAsset(const AssetMetadata& metadata)
	{
		AssetHandle handle; // generate handle
		m_AssetRegistry[handle] = metadata;
		return handle;
	}

}