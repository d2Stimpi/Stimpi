#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/Prefab.h"

namespace Stimpi
{
	class Entity;

	class ST_API PrefabManager
	{
	public:
		static Entity InstantiatePrefab(AssetHandle prefabHandle);

		static bool IsEntityValidPrefab(Entity entity);
		static Entity GetPrefabRootEntity(Entity entity);

		static void OnPrefabAssetReload(std::shared_ptr<Asset> asset);

		static void ConvertToPrefabEntity(Entity entity, AssetHandle prefabHandle, bool isRootObject = false);
	};
}