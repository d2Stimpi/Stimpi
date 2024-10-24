#include "stpch.h"
#include "Stimpi/Scene/Entity.h"

#include "Stimpi/Scene/Component.h"

#define SERIALIZE_COMPONENT(ComponentName)					\
		if (HasComponent<ComponentName>())					\
		{													\
			GetComponent<ComponentName>().Serialize(out);	\
		}

namespace Stimpi
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene)
	{

	}

	void Entity::Serialize(YAML::Emitter& out)
	{
		out << YAML::BeginMap;

		SERIALIZE_COMPONENT(UUIDComponent);
		SERIALIZE_COMPONENT(TagComponent);
		SERIALIZE_COMPONENT(QuadComponent);
		SERIALIZE_COMPONENT(CircleComponent);
		SERIALIZE_COMPONENT(SpriteComponent);
		SERIALIZE_COMPONENT(SortingGroupComponent);
		SERIALIZE_COMPONENT(AnimatedSpriteComponent);
		SERIALIZE_COMPONENT(ScriptComponent);
		SERIALIZE_COMPONENT(CameraComponent);
		SERIALIZE_COMPONENT(RigidBody2DComponent);
		SERIALIZE_COMPONENT(BoxCollider2DComponent);

		out << YAML::EndMap;
	}
}
