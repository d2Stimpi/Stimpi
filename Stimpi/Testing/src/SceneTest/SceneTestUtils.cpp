#include "stpch.h"
#include "SceneTest/SceneTestUtils.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"

namespace StimpiTest
{
	static void AddComponentToEntity(Entity entity, ComponentType componentType)
	{
		if (entity)
		{
			switch (componentType)
			{
			case ComponentType::ANIMATED_SPRITE:	entity.AddComponent<AnimatedSpriteComponent>(); break;
			case ComponentType::BOX_COLLIDER_2D:	entity.AddComponent<BoxCollider2DComponent>(); break;
			case ComponentType::CAMERA:				entity.AddComponent<CameraComponent>(); break;
			case ComponentType::CIRCLE:				entity.AddComponent<CircleComponent>(); break;
			case ComponentType::NATIVE_SCRIPT:		entity.AddComponent<NativeScriptComponent>(); break;
			case ComponentType::QUAD:				entity.AddComponent<QuadComponent>(); break;
			case ComponentType::RIGID_BODY_2D:		entity.AddComponent<RigidBody2DComponent>(); break;
			case ComponentType::SCRIPT:				entity.AddComponent<ScriptComponent>(); break;
			case ComponentType::SORTING_GROUP:		entity.AddComponent<SortingGroupComponent>(); break;
			case ComponentType::SPRITE:				entity.AddComponent<SpriteComponent>(); break;
			}
		}
	}


	EntityFactory* EntityFactory::Instance()
	{
		static std::unique_ptr<EntityFactory> instance = std::make_unique<EntityFactory>();
		return instance.get();
	}

	void EntityFactory::SetContext(Scene* scene)
	{
		EntityFactory::Instance()->m_Scene = scene;
	}

	Entity EntityFactory::CreateEntity(EntityComponentList list)
	{
		return EntityFactory::Instance()->CreateEntityI(list);
	}

	Entity EntityFactory::CreateEntityI(EntityComponentList list)
	{
		if (m_Scene)
		{
			Entity entity = m_Scene->CreateEntity("");

			for (auto type : list)
			{
				AddComponentToEntity(entity, type);
			}

			return entity;
		}

		return 0;
	}

}