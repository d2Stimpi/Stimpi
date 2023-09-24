#pragma once

#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Log.h"

#include <entt/entt.hpp>


namespace Stimpi
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		void Serialize();

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ST_CORE_ASSERT(HasComponent<T>(), "Entity already has component");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			ST_CORE_ASSERT(!HasComponent<T>(), "Entity does not have component");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ST_CORE_ASSERT(!HasComponent<T>(), "Entity does not have component");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

		operator bool() const { return (uint32_t)m_Handle != 0; }
	private:
		entt::entity m_Handle{ 0 };
		Scene* m_Scene{ nullptr };
	};
}