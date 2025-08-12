#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/Prefab.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	class Entity;

	class ST_API PrefabManager
	{
	public:
		static Entity InstantiatePrefab(AssetHandle prefabHandle, glm::vec3 position = {-10000.0f, -10000.0f, -10.0f});

		static bool IsEntityValidPrefab(Entity entity);

		static Entity GetPrefabRootEntity(Entity entity);
		static std::vector<Entity> GetAllPrefabEntities(const AssetHandle& prefabHandle);

		static void OnPrefabAssetReload(std::shared_ptr<Asset> asset);
	};
}