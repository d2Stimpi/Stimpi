#include "stpch.h"
#include "Stimpi/Asset/AssetManagerEditor.h"
#include "Stimpi/Asset/AssetImporter.h"

namespace Stimpi
{
	std::shared_ptr<AssetNew> AssetManagerEditor::GetAsset(AssetHandleNew handle)
	{
		std::shared_ptr<AssetNew> asset;

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

	bool AssetManagerEditor::IsAssetHandleValid(AssetHandleNew handle)
	{
		return handle != 0 && (m_AssetRegistry.find(handle) != m_AssetRegistry.end());
	}

	bool AssetManagerEditor::IsAssetLoaded(AssetHandleNew handle)
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	Stimpi::AssetMetadata& AssetManagerEditor::GetAssetMetadata(AssetHandleNew handle)
	{
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
			return s_NullMetadata;

		return it->second;
	}

	AssetHandleNew AssetManagerEditor::RegisterAsset(const AssetMetadata& metadata)
	{
		AssetHandleNew handle; // generate handle
		m_AssetRegistry[handle] = metadata;
		return handle;
	}

}