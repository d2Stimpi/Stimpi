#include "Entity.h"

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

		if (HasComponent<TextureComponent>())
		{
			GetComponent<TextureComponent>().Serialize(out);
		}
		out << YAML::EndMap;
	}
}