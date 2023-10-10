#pragma once

#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Log.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(Scene* scene) : m_Scene(scene) {}
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		void Serialize(YAML::Emitter& out);

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

		// TODO: remove when GUID is implemented
		operator uint32_t() const { return (uint32_t)m_Handle; }
	private:
		entt::entity m_Handle{ 0 };
		Scene* m_Scene{ nullptr };
	};
}