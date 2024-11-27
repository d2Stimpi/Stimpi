#include "stpch.h"
#include "Stimpi/Scene/EntityManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	struct EntityManagerContext
	{
		bool m_SortingLayerChanged = true;	// To trigger initial sort by Grouping components
	};

	static EntityManagerContext s_Context;

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

		// Set sort flag
		s_Context.m_SortingLayerChanged = true;
	}

	void EntityManager::UpdateEntitySortingLayerIndex(Entity entity)
	{
		if (entity.HasComponent<SortingGroupComponent>())
		{
			DefaultGroupComponent& def = entity.GetComponent<DefaultGroupComponent>();
			SortingGroupComponent& sor = entity.GetComponent<SortingGroupComponent>();

			def.m_LayerIndex = sor.m_LayerIndex;

			// Set sort flag
			s_Context.m_SortingLayerChanged = true;
		}
	}

	bool EntityManager::ShouldSortByGroupingLayersOrder()
	{
		bool resort = s_Context.m_SortingLayerChanged;
		s_Context.m_SortingLayerChanged = false;
		return resort;
	}

	void EntityManager::TriggerSortByGroupingLayersOrder()
	{
		s_Context.m_SortingLayerChanged = true;
	}

	void EntityManager::OnSceneCreated()
	{
		s_Context.m_SortingLayerChanged = true;
	}

}