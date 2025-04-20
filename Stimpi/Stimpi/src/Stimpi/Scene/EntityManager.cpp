#include "stpch.h"
#include "Stimpi/Scene/EntityManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{

	void EntityManager::UpdateSortingLayerIndexing()
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);

		uint32_t defaultIndex = Project::GetDefaultSortingLayerIndex();

		auto sortingView = scene->m_Registry.view<SortingGroupComponent, DefaultGroupComponent>();
		sortingView.each([](SortingGroupComponent& sor, DefaultGroupComponent& def)
			{
				sor.UpdateLayerIndex();
				def.m_LayerIndex = sor.m_LayerIndex;
			});

		auto defaultView = scene->m_Registry.view<DefaultGroupComponent>(entt::exclude<SortingGroupComponent>);
		defaultView.each([&](DefaultGroupComponent& def)
			{
				def.m_LayerIndex = defaultIndex;
			});
	}

	void EntityManager::UpdateEntitySortingLayerIndex(Entity entity)
	{
		if (entity.HasComponent<SortingGroupComponent>())
		{
			DefaultGroupComponent& def = entity.GetComponent<DefaultGroupComponent>();
			SortingGroupComponent& sor = entity.GetComponent<SortingGroupComponent>();

			def.m_LayerIndex = sor.m_LayerIndex;
			def.m_OrderInLayer = sor.m_OrderInLayer;
		}
	}

	void EntityManager::Translate(Entity entity, const glm::vec3& vec)
	{
		if (entity.HasComponent<QuadComponent>())
		{
			QuadComponent& quad = entity.GetComponent<QuadComponent>();
			quad.m_Position += vec;
		}

		if (entity.HasComponent<HierarchyComponent>())
		{
			HierarchyComponent& hierarcy = entity.GetComponent<HierarchyComponent>();
			if (!hierarcy.m_Children.empty())
			{
				auto scene = entity.GetScene();
				for (auto& uuid : hierarcy.m_Children)
				{
					Entity& childEntity = scene->m_EntityUUIDMap[uuid];
					Translate(childEntity, vec);
				}
			}
		}
	}

	void EntityManager::Translate(std::vector<Entity>& entities, const glm::vec3& vec)
	{
		for (auto& entity : entities)
		{
			if (entity.HasComponent<QuadComponent>())
			{
				QuadComponent& quad = entity.GetComponent<QuadComponent>();
				quad.m_Position += vec;
			}
		}
	}

}