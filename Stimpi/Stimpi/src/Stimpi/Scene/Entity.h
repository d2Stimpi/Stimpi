#pragma once

#include "Scene.h"
#include "Component.h"

#include <entt/entt.hpp>


namespace Stimpi
{
	class Entity
	{
	public:
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

	private:
		entt::entity m_Handle;
		Scene* m_Scene;
	};
}