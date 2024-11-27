#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class Entity;

	class ST_API EntityManager
	{
	public:
		static void UpdateSortingLayerIndexing();
		static void UpdateEntitySortingLayerIndex(Entity entity);

		/* Sort flag will be reset when this function returns */
		static bool ShouldSortByGroupingLayersOrder();
		static void TriggerSortByGroupingLayersOrder();

		static void OnSceneCreated();
	};
}