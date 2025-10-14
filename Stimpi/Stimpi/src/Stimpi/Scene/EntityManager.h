#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	class Entity;

	class ST_API EntityManager
	{
	public:
		static void UpdateSortingLayerIndexing();
		static void UpdateEntitySortingLayerIndex(Entity entity);

		static void Translate(Entity entity, const glm::vec3& vec);
		static void Translate(std::vector<Entity>& entities, const glm::vec3& vec);
		static void Scale(Entity entity, const glm::vec2& vec);
		static void Scale(std::vector<Entity>& entities, const glm::vec2& vec);
		static void Rotate(Entity entity, const float& val);
		static void Rotate(std::vector<Entity>& entities, const float& val);

		// Prefab related
		static void ConvertToPrefabEntity(Entity entity, AssetHandle prefabHandle, bool isRootObject = false);

		// Generic entity handling
		static void RemoveEntity(Entity entity);
		static std::vector<Entity> GetAllEntitiesInHierarchy(Entity parentEntity);

		// Render Entities
		static void SubmitForRendering(std::vector<Entity>& entities);
	};
}