#include "stpch.h"
#include "Stimpi/Asset/Prefab.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Component.h"

#define SERIALIZE_ENTITY_COMPONENT(Entity, ComponentName)			\
		if (Entity.HasComponent<ComponentName>())					\
		{															\
			Entity.GetComponent<ComponentName>().Serialize(out);	\
		}

#define REMOVE_ENTITY_COMPONENT(Entity, ComponentName)	\
		if (Entity.HasComponent<ComponentName>())		\
		{												\
			Entity.RemoveComponent<ComponentName>();	\
		}

#define BUILD_ENTITY_COMPONENT(Entity, ComponentName)	\
		if (m_Data[#ComponentName])						\
		{												\
			Entity.AddComponent<ComponentName>(ComponentName(m_Data[#ComponentName]));	\
		}

namespace Stimpi
{

	Prefab::Prefab(Entity entity)
	{
		Initialize(entity);
	}

	Prefab::Prefab(YAML::Node data)
		: m_Data(data)
	{

	}

	void Prefab::Initialize(Entity entity)
	{
		YAML::Emitter out;
		
		out << YAML::BeginMap;

		SERIALIZE_ENTITY_COMPONENT(entity, TagComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, HierarchyComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, QuadComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, CircleComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, SpriteComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, SortingGroupComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, AnimatedSpriteComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, ScriptComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, CameraComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, RigidBody2DComponent);
		SERIALIZE_ENTITY_COMPONENT(entity, BoxCollider2DComponent);

		out << YAML::EndMap;

		m_Data = YAML::Load(out.c_str());
	}

	void Prefab::Save(const FilePath& filePath)
	{
		ResourceManager::Instance()->WriteToFile(filePath, m_Data);
	}

	void Prefab::BuildComponents(Entity entity)
	{
		std::string tag = "Prefab";
		if (m_Data["TagComponent"])
		{
			tag = m_Data["TagComponent"]["Tag"].as<std::string>();
		}
		entity.AddComponent<TagComponent>(tag);
		BUILD_ENTITY_COMPONENT(entity, QuadComponent);
		BUILD_ENTITY_COMPONENT(entity, HierarchyComponent);
		BUILD_ENTITY_COMPONENT(entity, CircleComponent);
		BUILD_ENTITY_COMPONENT(entity, SpriteComponent);
		BUILD_ENTITY_COMPONENT(entity, SortingGroupComponent);
		BUILD_ENTITY_COMPONENT(entity, AnimatedSpriteComponent);
		if (m_Data["ScriptComponent"])
		{
			ScriptComponent& scriptComponent = entity.AddComponent<ScriptComponent>(ScriptComponent(m_Data["ScriptComponent"]));
			scriptComponent.PopulateScriptInstanceData(m_Data["ScriptComponent"]);
		}
		BUILD_ENTITY_COMPONENT(entity, CameraComponent);
		BUILD_ENTITY_COMPONENT(entity, RigidBody2DComponent);
		BUILD_ENTITY_COMPONENT(entity, BoxCollider2DComponent);

		// Only prefabs will have PrefabComponent as a way to identify them easily
		if (!entity.HasComponent<PrefabComponent>())
			entity.AddComponent<PrefabComponent>(m_Handle);
	}

	void Prefab::UpdateComponents(Entity entity)
	{
		// Save the current entity position in order to restore it after update.
		// Provided that the entity has QuadComponent
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		if (entity.HasComponent<QuadComponent>())
		{
			QuadComponent& quad = entity.GetComponent<QuadComponent>();
			position = quad.m_Position;
		}

		REMOVE_ENTITY_COMPONENT(entity, TagComponent)
		REMOVE_ENTITY_COMPONENT(entity, HierarchyComponent);
		REMOVE_ENTITY_COMPONENT(entity, QuadComponent);
		REMOVE_ENTITY_COMPONENT(entity, CircleComponent);
		REMOVE_ENTITY_COMPONENT(entity, SpriteComponent);
		REMOVE_ENTITY_COMPONENT(entity, SortingGroupComponent);
		REMOVE_ENTITY_COMPONENT(entity, AnimatedSpriteComponent);
		REMOVE_ENTITY_COMPONENT(entity, ScriptComponent);
		REMOVE_ENTITY_COMPONENT(entity, CameraComponent);
		REMOVE_ENTITY_COMPONENT(entity, RigidBody2DComponent);
		REMOVE_ENTITY_COMPONENT(entity, BoxCollider2DComponent);

		BuildComponents(entity);

		// Restore the entity's position
		if (entity.HasComponent<QuadComponent>())
		{
			QuadComponent& quad = entity.GetComponent<QuadComponent>();
			quad.m_Position = position;
		}
	}

	std::shared_ptr<Prefab> Prefab::Create()
	{
		return std::make_shared<Prefab>();
	}

	std::shared_ptr<Prefab> Prefab::Create(Entity entity)
	{
		return std::make_shared<Prefab>(entity);
	}

	std::shared_ptr<Prefab> Prefab::Create(const FilePath& filePath)
	{
		YAML::Node data = YAML::LoadFile(filePath);
		if (data.IsDefined())
		{
			return std::make_shared<Prefab>(data);
		}

		return nullptr;
	}

}