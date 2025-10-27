#include "stpch.h"
#include "Stimpi/Asset/SceneAssetBuilder.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> SceneAssetBuilder::CreateSceneAsset(const AssetMetadata& metadata)
	{
		std::shared_ptr<Scene> sceneAsset = std::make_shared<Scene>();
		return std::static_pointer_cast<Asset>(sceneAsset);
	}

}