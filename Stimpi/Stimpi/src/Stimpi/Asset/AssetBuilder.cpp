#include "stpch.h"
#include "Stimpi/Asset/AssetBuilder.h"

#include "Stimpi/Asset/PrefabAssetBuilder.h"
#include "Stimpi/Asset/SceneAssetBuilder.h"

namespace Stimpi
{

	using AssetBuilderFunction = std::function<std::shared_ptr<Asset>(const AssetMetadata&)>;

	std::unordered_map<AssetType, AssetBuilderFunction> s_AssetBuilders =
	{
		{ AssetType::PREFAB, PrefabAssetBuilder::CreatePrefabAsset },
		{ AssetType::SCENE, SceneAssetBuilder::CreateSceneAsset }
	};

	std::shared_ptr<Asset> AssetBuilder::CreateAsset(AssetMetadata& metadata)
	{
		if (s_AssetBuilders.find(metadata.m_Type) == s_AssetBuilders.end())
		{
			ST_CORE_ERROR("AssetBuilder::CreateAsset: Unknown asset type!");
			return nullptr;
		}

		return s_AssetBuilders.at(metadata.m_Type)(metadata);
	}

}