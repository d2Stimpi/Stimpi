#include "stpch.h"
#include "Stimpi/Scene/EntityHierarchy.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

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
		
		// If not already added
		/*if (std::find(parentComponent.m_Children.begin(), parentComponent.m_Children.end(), childUUID) == parentComponent.m_Children.end())
		{
			parentComponent.m_Children.push_back(childUUID);
		}*/

		if (childComponent.m_Parent != parentUUID)
		{
			if (childComponent.m_Parent)
			{
				auto scene = SceneManager::Instance()->GetActiveScene();
				Entity& prePrent = scene->m_EntityUUIDMap[childComponent.m_Parent];

				RemoveChild(prePrent, child);
			}

			parentComponent.m_Children.push_back(childUUID);
			childComponent.m_Parent = parentUUID;
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
			
			childComponent.m_Parent = UUID(0);

			// Remove component if all child entities were removed
			if (parentComponent.m_Children.empty())
				parent.RemoveComponent<HierarchyComponent>();
		}
	}

}