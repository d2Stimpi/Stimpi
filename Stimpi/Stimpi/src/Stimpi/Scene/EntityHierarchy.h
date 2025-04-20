#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class Entity;
	class Scene;

	class ST_API EntityHierarchy
	{
	public:
		static void AddChild(Entity& parent, Entity& child);
		static void RemoveChild(Entity& parent, Entity& child);
	};
}