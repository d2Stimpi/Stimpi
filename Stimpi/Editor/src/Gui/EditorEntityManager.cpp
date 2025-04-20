#include "stpch.h"
#include "Gui/EditorEntityManager.h"

#include "Stimpi/Cmd/CommandStack.h"

namespace Stimpi
{
	struct EditorEntityManagerContext
	{
		glm::vec3 m_StartPos;
		bool m_StartPosRecorded = false;
	};

	static EditorEntityManagerContext s_Context;

	void EditorEntityManager::Translate(Entity entity, const glm::vec3& vec, bool recordCmd)
	{
		// No need to check again here
		QuadComponent& component = entity.GetComponent<QuadComponent>();
		
		if (s_Context.m_StartPosRecorded == false)
		{
			s_Context.m_StartPos = component.m_Position;
			s_Context.m_StartPosRecorded = true;
		}

		if (recordCmd)
		{
			Command* cmd = EntityHierarchyCommand::Create(
				entity,
				HierarchyCommandType::TRANSLATE,
				component.m_Position - s_Context.m_StartPos);
			CommandStack::Push(cmd);

			// When done recording command, reset the flag
			s_Context.m_StartPosRecorded = false;
		}

		// Perform entity move
		EntityManager::Translate(entity, vec);
	}

	void EditorEntityManager::Translate(std::vector<Entity>& entities, const glm::vec3& vec, bool recordCmd)
	{
		EntityManager::Translate(entities, vec);
	}

}