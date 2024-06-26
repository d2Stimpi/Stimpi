#include "stpch.h"
#include "Stimpi/Scene/Entity.h"

#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene)
	{

	}

	void Entity::Serialize(YAML::Emitter& out)
	{
		out << YAML::BeginMap;
		if (HasComponent<TagComponent>())
		{
			GetComponent<TagComponent>().Serialize(out);
		}

		if (HasComponent<QuadComponent>())
		{
			GetComponent<QuadComponent>().Serialize(out);
		}

		if (HasComponent<CircleComponent>())
		{
			GetComponent<CircleComponent>().Serialize(out);
		}

		if (HasComponent<SpriteComponent>())
		{
			GetComponent<SpriteComponent>().Serialize(out);
		}

		if (HasComponent<SortingGroupComponent>())
		{
			GetComponent<SortingGroupComponent>().Serialize(out);
		}

		if (HasComponent<AnimatedSpriteComponent>())
		{
			GetComponent<AnimatedSpriteComponent>().Serialize(out);
		}

		if (HasComponent<ScriptComponent>())
		{
			GetComponent<ScriptComponent>().Serialize(out);
		}

		if (HasComponent<CameraComponent>())
		{
			GetComponent<CameraComponent>().Serialize(out);
		}

		if (HasComponent<RigidBody2DComponent>())
		{
			GetComponent<RigidBody2DComponent>().Serialize(out);
		}

		if (HasComponent<BoxCollider2DComponent>())
		{
			GetComponent<BoxCollider2DComponent>().Serialize(out);
		}
		out << YAML::EndMap;
	}
}
