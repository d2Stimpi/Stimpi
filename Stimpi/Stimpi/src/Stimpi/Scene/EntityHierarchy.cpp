#include "stpch.h"
#include "Stimpi/Scene/EntityHierarchy.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stimpi
{

	void EntityHierarchy::AddChild(Entity& parent, Entity& child)
	{
		UUID parentUUID = parent.GetComponent<UUIDComponent>().m_UUID;
		UUID childUUID = child.GetComponent<UUIDComponent>().m_UUID;

		if (!parent.HasComponent<HierarchyComponent>())
			parent.AddComponent<HierarchyComponent>();
		if (!child.HasComponent<HierarchyComponent>())
			child.AddComponent<HierarchyComponent>();

		HierarchyComponent& parentComponent = parent.GetComponent<HierarchyComponent>();
		HierarchyComponent& childComponent = child.GetComponent<HierarchyComponent>();

		// Prevent adding if already the same parent, prevent adding
		if (childComponent.m_Parent != parentUUID && parentComponent.m_Parent != childUUID)
		{
			UUID prevParentUUID = childComponent.m_Parent;

			parentComponent.m_Children.push_back(childUUID);
			childComponent.m_Parent = parentUUID;

			if (prevParentUUID)
			{
				auto scene = SceneManager::Instance()->GetActiveScene();
				Entity& preParent = scene->m_EntityUUIDMap[prevParentUUID];

				RemoveChild(preParent, child);
			}
		}

	}

	void EntityHierarchy::RemoveChild(Entity& parent, Entity& child)
	{
		if (parent.HasComponent<HierarchyComponent>() && child.HasComponent<HierarchyComponent>())
		{
			UUID parentUUID = parent.GetComponent<UUIDComponent>().m_UUID;
			UUID childUUID = child.GetComponent<UUIDComponent>().m_UUID;
			HierarchyComponent& parentComponent = parent.GetComponent<HierarchyComponent>();
			HierarchyComponent& childComponent = child.GetComponent<HierarchyComponent>();

			parentComponent.m_Children.erase(std::find(parentComponent.m_Children.begin(), parentComponent.m_Children.end(), childUUID));

			// Remove component if all child entities were removed
			if (parentComponent.m_Children.empty() && parentComponent.m_Parent == 0)
				parent.RemoveComponent<HierarchyComponent>();
		}
	}

}