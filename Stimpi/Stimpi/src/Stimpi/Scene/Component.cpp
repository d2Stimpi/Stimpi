#include "stpch.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/EntityManager.h"

namespace Stimpi
{
	Scene* s_ActiveScene = nullptr;

	static void OnQuadConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		if (entity.HasComponent<CameraComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = false;
		}
	}

	static void OnQuadDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, s_ActiveScene };
	}

	static void OnCircleConstruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, s_ActiveScene };
	}

	static void OnCircleDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, s_ActiveScene };
	}

	static void OnCameraConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = false;
		}
	}

	static void OnCameraDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = true;
		}
	}

	// Script
	static void OnScriptConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };

		if (entity.HasComponent<ScriptComponent>())
		{
			ScriptComponent& component = entity.GetComponent<ScriptComponent>();
			if (!component.m_ScriptName.empty())
			{
				component.m_ScriptInstance = ScriptEngine::OnScriptComponentAdd(component.m_ScriptName, entity);
			}
		}
	}

	//TODO: scriptOnDestruct

	// Sprite
	static void OnSpriteConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
	}

	static void OnSpriteDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		
		SpriteComponent sprite = entity.GetComponent<SpriteComponent>();
		if (sprite.m_TextureHandle.IsValid())
			AssetManager::Release(sprite.m_TextureHandle);
	}

	// AnimatedSprite
	static void OnAnimatedSpriteConstruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, s_ActiveScene };
	}

	static void OnAnimatedSpriteDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, s_ActiveScene };
	}

	// RigidBody2D
	static void OnRigidBody2DConstruct(entt::registry& reg, entt::entity ent)
	{

	}

	static void OnRigidBody2DDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		s_ActiveScene->DestroyPhysicsBody(entity);
	}

	// SortingGroup
	static void OnSortingGroupConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		
		SortingGroupComponent sortingGroup = entity.GetComponent<SortingGroupComponent>();
		DefaultGroupComponent& component = entity.GetComponent<DefaultGroupComponent>();
		component.m_LayerIndex = sortingGroup.m_LayerIndex;
		component.m_OrderInLayer = sortingGroup.m_OrderInLayer;
	}

	static void OnSortingGroupDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		if (entity.HasComponent<DefaultGroupComponent>())
		{
			DefaultGroupComponent& component = entity.GetComponent<DefaultGroupComponent>();
			component.m_LayerIndex = Project::GetDefaultSortingLayerIndex();
			component.m_OrderInLayer = 0;
		}
	}

	// DefaultGroupComponent
	static void OnDefaultGroupComponentConstruct(entt::registry& reg, entt::entity ent)
	{
	}

	static void OnDefaultGroupComponentDestruct(entt::registry& reg, entt::entity ent)
	{
	}

#define ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(component, function)	reg.on_construct<component>().connect<&function>()
#define ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(component, function)		reg.on_construct<component>().disconnect<&function>()
#define ENTT_REGISTER_COMPONENT_ON_DESTROY(component, function)		reg.on_destroy<component>().connect<&function>()
#define ENTT_REMOVE_COMPONENT_ON_DESTROY(component, function)		reg.on_destroy<component>().disconnect<&function>()

	void ComponentObserver::InitComponentObservers(entt::registry& reg, Scene* scene)
	{
		s_ActiveScene = scene;

		// on_construct
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(QuadComponent, OnQuadConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(CircleComponent, OnCircleConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(CameraComponent, OnCameraConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(ScriptComponent, OnScriptConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(SpriteComponent, OnSpriteConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(AnimatedSpriteComponent, OnAnimatedSpriteConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(SortingGroupComponent, OnSortingGroupConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(DefaultGroupComponent, OnDefaultGroupComponentConstruct);

		// on_destroy
		ENTT_REGISTER_COMPONENT_ON_DESTROY(QuadComponent, OnQuadDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(CircleComponent, OnCircleDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(CameraComponent, OnCameraDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(SpriteComponent, OnSpriteDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(AnimatedSpriteComponent, OnAnimatedSpriteDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(RigidBody2DComponent, OnRigidBody2DDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(SortingGroupComponent, OnSortingGroupDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(DefaultGroupComponent, OnDefaultGroupComponentDestruct);
	}

	void ComponentObserver::DeinitConstructObservers(entt::registry& reg)
	{
		// on_construct
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(QuadComponent, OnQuadConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(CircleComponent, OnCircleConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(CameraComponent, OnCameraConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(ScriptComponent, OnScriptConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(SpriteComponent, OnSpriteConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(AnimatedSpriteComponent, OnAnimatedSpriteConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(SortingGroupComponent, OnSortingGroupConstruct);

		// on_destroy
		ENTT_REMOVE_COMPONENT_ON_DESTROY(QuadComponent, OnQuadDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(CircleComponent, OnCircleDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(CameraComponent, OnCameraDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(SpriteComponent, OnSpriteDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(AnimatedSpriteComponent, OnAnimatedSpriteDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(RigidBody2DComponent, OnRigidBody2DDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(SortingGroupComponent, OnSortingGroupDestruct);
	}

}