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
	};
}