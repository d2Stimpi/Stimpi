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
	};
}