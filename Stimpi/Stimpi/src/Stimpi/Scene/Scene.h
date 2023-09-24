#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Timestep.h"

#include <entt/entt.hpp>


namespace Stimpi
{
	class Entity;

	class ST_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);

		Entity CreateEntity();

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}