#include "stpch.h"
#include "Gui/EditorEntityManager.h"

#include "Stimpi/Cmd/CommandStack.h"

namespace Stimpi
{

	void EditorEntityManager::Translate(Entity entity, const glm::vec3& vec)
	{
		EntityManager::Translate(entity, vec);
	}

	void EditorEntityManager::Translate(std::vector<Entity>& entities, const glm::vec3& vec)
	{
		EntityManager::Translate(entities, vec);
	}

}