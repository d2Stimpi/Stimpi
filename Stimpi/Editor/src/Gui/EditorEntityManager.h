#pragma once

#include "Stimpi/Scene/EntityManager.h"

namespace Stimpi
{
	class Entity;

	class EditorEntityManager : public EntityManager
	{
	public:

		static void Translate(Entity entity, const glm::vec3& vec, bool recordCmd = true);
		static void Translate(std::vector<Entity>& entities, const glm::vec3& vec, bool recordCmd = true);
	};
}