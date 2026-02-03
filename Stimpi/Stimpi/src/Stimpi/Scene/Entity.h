#pragma once

#include "Stimpi/Core/UUID.h"
#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Log.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

#define INVALID_ENTITY_HANDLE	0

namespace Stimpi
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(Scene* scene) : m_Scene(scene) {}
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;
		
		Entity(Entity&& other) : m_Handle(std::move(other.m_Handle)), m_Scene(std::move(other.m_Scene)) { other.m_Scene = nullptr; }
		Entity operator=(const Entity& other) { m_Handle = other.m_Handle; m_Scene = other.m_Scene; return *this; }

		void Serialize(YAML::Emitter& out);

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ST_CORE_ASSERT_MSG(HasComponent<T>(), "Entity already has component");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			ST_CORE_ASSERT_MSG(!HasComponent<T>(), "Entity does not have component");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponent()
		{
			ST_CORE_ASSERT_MSG(!HasComponent<T>(), "Entity does not have component");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_Handle);
		}

		entt::entity GetHandle() const { return m_Handle; }
		Scene* GetScene() { return m_Scene; }

		operator bool() const { return (uint32_t)m_Handle != 0; }
		bool operator== (const Entity& rhs) { return m_Handle == rhs.m_Handle; }

		// TODO: remove when GUID is implemented
		operator uint32_t() const { return (uint32_t)m_Handle; }
	private:
		entt::entity m_Handle{ INVALID_ENTITY_HANDLE };
		Scene* m_Scene{ nullptr };
	};
}