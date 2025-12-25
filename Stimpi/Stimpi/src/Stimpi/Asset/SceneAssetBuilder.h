#pragma once

#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class SceneAssetBuilder
	{
	public:
		static std::shared_ptr<Asset> CreateSceneAsset(const AssetMetadata& metadata);
	};
}