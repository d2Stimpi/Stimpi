#include "stpch.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/EntityManager.h"

namespace Stimpi
{

	void ComponentObserver::OnQuadConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
		if (entity.HasComponent<CameraComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = false;
		}
	}

	void ComponentObserver::OnQuadDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, m_Scene };
	}

	void ComponentObserver::OnCircleConstruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, m_Scene };
	}

	void ComponentObserver::OnCircleDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, m_Scene };
	}

	void ComponentObserver::OnCameraConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = false;
		}
	}

	void ComponentObserver::OnCameraDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_PickEnabled = true;
		}
	}

	// Script
	void ComponentObserver::OnScriptConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };

		if (entity.HasComponent<ScriptComponent>())
		{
			ScriptComponent& component = entity.GetComponent<ScriptComponent>();
			if (!component.m_ScriptName.empty())
			{
				component.m_ScriptInstance = ScriptEngine::OnScriptComponentAdd(component.m_ScriptName, entity);
			}
		}
	}

	void ComponentObserver::OnScriptDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };

		ScriptComponent& component = entity.GetComponent<ScriptComponent>();
		if (!component.m_ScriptInstance)
		{
			ScriptEngine::OnScriptComponentRemove(entity);
		}
	}

	//TODO: scriptOnDestruct

	// Sprite
	void ComponentObserver::OnSpriteConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
	}

	void ComponentObserver::OnSpriteDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
	}

	// AnimatedSprite
	void ComponentObserver::OnAnimatedSpriteConstruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, m_Scene };
	}

	void ComponentObserver::OnAnimatedSpriteDestruct(entt::registry& reg, entt::entity ent)
	{
		//Entity entity = { ent, m_Scene };
	}

	// RigidBody2D
	void ComponentObserver::OnRigidBody2DConstruct(entt::registry& reg, entt::entity ent)
	{

	}

	void ComponentObserver::OnRigidBody2DDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
		m_Scene->DestroyPhysicsBody(entity);
	}

	// SortingGroup
	void ComponentObserver::OnSortingGroupConstruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
		
		SortingGroupComponent sortingGroup = entity.GetComponent<SortingGroupComponent>();
		DefaultGroupComponent& component = entity.GetComponent<DefaultGroupComponent>();
		component.m_LayerIndex = sortingGroup.m_LayerIndex;
		component.m_OrderInLayer = sortingGroup.m_OrderInLayer;
	}

	void ComponentObserver::OnSortingGroupDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, m_Scene };
		if (entity.HasComponent<DefaultGroupComponent>())
		{
			DefaultGroupComponent& component = entity.GetComponent<DefaultGroupComponent>();
			component.m_LayerIndex = Project::GetDefaultSortingLayerIndex();
			component.m_OrderInLayer = 0;
		}
	}

	// DefaultGroupComponent
	void ComponentObserver::OnDefaultGroupComponentConstruct(entt::registry& reg, entt::entity ent)
	{
	}

	void ComponentObserver::OnDefaultGroupComponentDestruct(entt::registry& reg, entt::entity ent)
	{
	}

#define ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(component, function)	reg.on_construct<component>().connect<&function>(this)
#define ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(component, function)		reg.on_construct<component>().disconnect<&function>(this)
#define ENTT_REGISTER_COMPONENT_ON_DESTROY(component, function)		reg.on_destroy<component>().connect<&function>(this)
#define ENTT_REMOVE_COMPONENT_ON_DESTROY(component, function)		reg.on_destroy<component>().disconnect<&function>(this)

	static void TestMethod(entt::registry& reg, entt::entity ent)
	{

	}

	void ComponentObserver::InitComponentObservers(entt::registry& reg, Scene* scene)
	{
		m_Scene = scene;

		// on_construct
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(QuadComponent, ComponentObserver::OnQuadConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(CircleComponent, ComponentObserver::OnCircleConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(CameraComponent, ComponentObserver::OnCameraConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(ScriptComponent, ComponentObserver::OnScriptConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(SpriteComponent, ComponentObserver::OnSpriteConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(AnimatedSpriteComponent, ComponentObserver::OnAnimatedSpriteConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(SortingGroupComponent, ComponentObserver::OnSortingGroupConstruct);
		ENTT_REGISTER_COMPONENT_ON_CONSTRUCT(DefaultGroupComponent, ComponentObserver::OnDefaultGroupComponentConstruct);
		
		// on_destroy
		ENTT_REGISTER_COMPONENT_ON_DESTROY(QuadComponent, ComponentObserver::OnQuadDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(CircleComponent, ComponentObserver::OnCircleDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(CameraComponent, ComponentObserver::OnCameraDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(ScriptComponent, ComponentObserver::OnScriptDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(SpriteComponent, ComponentObserver::OnSpriteDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(AnimatedSpriteComponent, ComponentObserver::OnAnimatedSpriteDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(RigidBody2DComponent, ComponentObserver::OnRigidBody2DDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(SortingGroupComponent, ComponentObserver::OnSortingGroupDestruct);
		ENTT_REGISTER_COMPONENT_ON_DESTROY(DefaultGroupComponent, ComponentObserver::OnDefaultGroupComponentDestruct);
	}

	void ComponentObserver::DeinitConstructObservers(entt::registry& reg)
	{
		// on_construct
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(QuadComponent, ComponentObserver::OnQuadConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(CircleComponent, ComponentObserver::OnCircleConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(CameraComponent, ComponentObserver::OnCameraConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(ScriptComponent, ComponentObserver::OnScriptConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(SpriteComponent, ComponentObserver::OnSpriteConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(AnimatedSpriteComponent, ComponentObserver::OnAnimatedSpriteConstruct);
		ENTT_REMOVE_COMPONENT_ON_CONSTRUCT(SortingGroupComponent, ComponentObserver::OnSortingGroupConstruct);

		// on_destroy
		ENTT_REMOVE_COMPONENT_ON_DESTROY(QuadComponent, ComponentObserver::OnQuadDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(CircleComponent, ComponentObserver::OnCircleDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(CameraComponent, ComponentObserver::OnCameraDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(SpriteComponent, ComponentObserver::OnSpriteDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(AnimatedSpriteComponent, ComponentObserver::OnAnimatedSpriteDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(RigidBody2DComponent, ComponentObserver::OnRigidBody2DDestruct);
		ENTT_REMOVE_COMPONENT_ON_DESTROY(SortingGroupComponent, ComponentObserver::OnSortingGroupDestruct);
	}

}

YAML::Emitter& operator<<(YAML::Emitter& out, const Stimpi::QuadComponent& quad)
{
	out << YAML::BeginSeq;
	out << quad.m_Position.x << quad.m_Position.y << quad.m_Position.z << quad.m_Size.x << quad.m_Size.y << quad.m_Rotation;
	out << YAML::EndSeq;
	return out;
}