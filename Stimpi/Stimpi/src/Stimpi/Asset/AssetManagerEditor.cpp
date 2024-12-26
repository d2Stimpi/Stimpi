#include "stpch.h"
#include "Stimpi/Asset/AssetManagerEditor.h"

#include "Stimpi/Asset/AssetImporter.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

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

	void AssetManagerEditor::SerializeAssetRegistry(const FilePath& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "AssetRegistry" << YAML::Value;
			out << YAML::BeginMap;
			{
				for (auto& item : m_AssetRegistry)
				{
					out << YAML::Key << "Asset" << YAML::Value;
					out << YAML::BeginMap;
					{
						out << YAML::Key << "Handle" << YAML::Value << item.first;
						AssetMetadata metadata = item.second;
						out << YAML::Key << "Metadata" << YAML::Value;
						out << YAML::BeginMap;
						{
							out << YAML::Key << "AssetType" << YAML::Value << AssetTypeToString(metadata.m_Type);
							out << YAML::Key << "FilePath" << YAML::Value << metadata.m_FilePath.string();
						}
						out << YAML::EndMap;
					}
					out << YAML::EndMap;
				}
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath.string(), out.c_str());
	}

	void AssetManagerEditor::DeserializeAssetRegistry(const FilePath& filePath)
	{

	}

}