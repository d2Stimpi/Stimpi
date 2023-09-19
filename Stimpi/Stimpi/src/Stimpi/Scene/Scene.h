#pragma once

//#include "Entity.h"

#include <entt/entt.hpp>


namespace Stimpi
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate();

		Entity CreateEntity();

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}