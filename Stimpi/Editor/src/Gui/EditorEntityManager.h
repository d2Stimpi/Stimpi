#pragma once

#include "Stimpi/Scene/EntityManager.h"

namespace Stimpi
{
	class Entity;

	enum class EntityType { DEFAULT, PREFAB };

	class EditorEntityManager : public EntityManager
	{
	public:

		static void Translate(Entity entity, const glm::vec3& vec);
		static void Translate(std::vector<Entity>& entities, const glm::vec3& vec);

		static EntityType GetEntityType(Entity entity);
	};
}