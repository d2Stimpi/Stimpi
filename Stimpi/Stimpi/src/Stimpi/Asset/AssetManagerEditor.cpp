#include "stpch.h"
#include "Stimpi/Asset/AssetManagerEditor.h"

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/AssetImporter.h"
#include "Stimpi/Asset/AssetBuilder.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Graphics/ShaderRegistry.h"
//temp
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Asset/Prefab.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	// Internal for path -> handle lookup (checking if asset is registered)
	std::unordered_map<std::string, AssetHandle> s_PathLookupRegistry;

	std::shared_ptr<Asset> AssetManagerEditor::GetAsset(AssetHandle handle)
	{
		std::shared_ptr<Asset> asset;

		if (!IsAssetHandleValid(handle))
		{
			ST_CORE_WARN("Invalid asset handle used: {}", handle);
			return nullptr;
		}
		
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			// Load asset or check if loading is finished (for async version)
			auto& metadata = GetAssetMetadata(handle);
			asset = AssetImporter::ImportAsset(handle, metadata);
			if (!asset)
			{
				ST_CORE_ERROR("Failed to import asset: {}", metadata.m_FilePath.string());
			}
			else
			{
				metadata.m_LastWriteTime = FileSystem::LastWriteTime(Project::GetAssestsDir() / metadata.m_FilePath);
				m_LoadedAssets[handle] = asset;

				asset->m_Handle = handle;
			}
		}

		return asset;
	}

	bool AssetManagerEditor::IsAssetHandleValid(AssetHandle handle)
	{
		return handle != INVALID_ASSET_HANDLE && (m_AssetRegistry.find(handle) != m_AssetRegistry.end());
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

	Stimpi::AssetHandle AssetManagerEditor::GetAssetHandle(const FilePath& filePath)
	{
		auto it = m_AssetLookup.find(filePath.string());
		if (it == m_AssetLookup.end())
			return 0;

		return it->second;
	}

	Stimpi::AssetHandle AssetManagerEditor::GetAssetHandle(const AssetMetadata& metadata)
	{
		return GetAssetHandle(metadata.m_FilePath);
	}

	bool AssetManagerEditor::WasAssetUpdated(AssetHandle handle)
	{
		static FileTimeType s_ZeroTime;
		AssetMetadata& metadata = GetAssetMetadata(handle);
		// Check if valid asset type - note: LastWriteTime is taken only when the asset is loaded for the first time and on each asset reload
		if (metadata.m_Type != AssetType::NONE && IsAssetLoaded(handle))
		{
			return metadata.m_LastWriteTime != FileSystem::LastWriteTime(Project::GetAssestsDir() / metadata.m_FilePath);
		}

		return false;
	}

	bool AssetManagerEditor::ReloadAsset(AssetHandle handle)
	{
		if (!IsAssetHandleValid(handle))
			return false;

		if (IsAssetLoaded(handle))
		{
			auto& metadata = GetAssetMetadata(handle);
			auto newAsset = AssetImporter::ImportAsset(handle, metadata);
			if (!newAsset)
			{
				ST_CORE_ERROR("Failed to import asset (Reload): {}", metadata.m_FilePath.string());
			}
			else
			{
				metadata.m_LastWriteTime = FileSystem::LastWriteTime(Project::GetAssestsDir() / metadata.m_FilePath);
				m_LoadedAssets[handle] = newAsset;
				newAsset->m_Handle = handle;

				for (auto& handler : m_AssetReloadHandlers)
				{
					handler->OnAssetReload(newAsset);
				}
			}
		}

		return false;
	}

	AssetHandle AssetManagerEditor::RegisterAsset(const AssetMetadata& metadata)
	{
		AssetHandle handle; // generate handle
		m_AssetRegistry[handle] = metadata;
		m_AssetLookup[metadata.m_FilePath.string()] = handle;

		OnAssetRegistered(handle, metadata);

		return handle;
	}

	bool AssetManagerEditor::IsAssetRegistered(const FilePath& filePath)
	{
		return m_AssetLookup.find(filePath.string()) != m_AssetLookup.end();
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
		if (!std::filesystem::exists(filePath.GetPath()))
		{
			ST_CORE_INFO("First time load project, no valid Asset Registry found");
			return;
		}

		YAML::Node loadData = YAML::LoadFile(filePath.string());
		YAML::Node registry = loadData["AssetRegistry"];
		ST_CORE_ASSERT_MSG(!registry, "Invalid AssetRegistry file!");

		for (YAML::const_iterator it = registry.begin(); it != registry.end(); it++)
		{
			YAML::Node assetNode = it->second;
			if (assetNode["Handle"] && assetNode["Metadata"])
			{
				AssetHandle handle = assetNode["Handle"].as<UUIDType>();
				YAML::Node metadataNode = assetNode["Metadata"];
				if (metadataNode["AssetType"] && metadataNode["FilePath"])
				{
					AssetMetadata metadata;
					metadata.m_Type = StringToAssetType(assetNode["Metadata"]["AssetType"].as<std::string>());
					metadata.m_FilePath = assetNode["Metadata"]["FilePath"].as<std::string>();
					m_AssetRegistry[handle] = metadata;
					m_AssetLookup[metadata.m_FilePath.string()] = handle;

					OnAssetRegistered(handle, metadata);
				}
				else
				{
					if (!metadataNode["AssetType"]) ST_CORE_ERROR("DeserializeAssetRegistry: Metadata malformed - missing AssetType!");
					if (!metadataNode["FilePath"]) ST_CORE_ERROR("DeserializeAssetRegistry: Metadata malformed - missing FilePath!");
				}
			}
			else
			{
				if (!assetNode["Handle"]) ST_CORE_ERROR("DeserializeAssetRegistry: invalid asset Handle!");
				if (!assetNode["Metadata"]) ST_CORE_ERROR("DeserializeAssetRegistry: invalid asset Metadata!");
			}
		}
	}

	size_t AssetManagerEditor::GetLoadedAssetsCount()
	{
		return m_LoadedAssets.size();
	}

	AssetHandle AssetManagerEditor::CreateAsset(AssetMetadata& metadata)
	{
		auto asset = AssetBuilder::CreateAsset(metadata);
		if (asset)
		{
			AssetHandle assetHandle = RegisterAsset(metadata);
			asset->m_Handle = assetHandle;
			m_LoadedAssets[assetHandle] = asset;
			return assetHandle;
		}

		return INVALID_ASSET_HANDLE;
	}

	void AssetManagerEditor::RegisterAssetReloadHandler(AssetReloadHandler* handler)
	{
		if (handler)
		{
			auto found = std::find(m_AssetReloadHandlers.begin(), m_AssetReloadHandlers.end(), handler);
			if (found == m_AssetReloadHandlers.end())
			{
				m_AssetReloadHandlers.push_back(handler);
			}
		}
	}

	void AssetManagerEditor::UnregisterAssetReloadHandler(AssetReloadHandler* handler)
	{
		if (handler)
		{
			m_AssetReloadHandlers.erase(std::remove(m_AssetReloadHandlers.begin(), m_AssetReloadHandlers.end(), handler), m_AssetReloadHandlers.end());
		}
	}

	void AssetManagerEditor::OnAssetRegistered(AssetHandle handle, AssetMetadata metadata)
	{
		if (metadata.m_Type == AssetType::SHADER)
		{
			ShaderRegistry::RegisterShader(handle);
		}
	}

}