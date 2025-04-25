#pragma once

#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/Prefab.h"

namespace Stimpi
{
	class Entity;

	class PrefabManager
	{
	public:
		static Entity InstantiatePrefab(AssetHandle prefabHandle);

		static void OnPrefabAssetReload(std::shared_ptr<Asset> asset);
	};
}