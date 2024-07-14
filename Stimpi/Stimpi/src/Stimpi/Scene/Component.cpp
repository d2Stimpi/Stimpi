#include "stpch.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

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

	// Sprite
	static void OnSpriteConstruct(entt::registry& reg, entt::entity ent)
	{

	}

	static void OnSpriteDestruct(entt::registry& reg, entt::entity ent)
	{
		Entity entity = { ent, s_ActiveScene };
		// TODO: release resources here
		SpriteComponent sprite = entity.GetComponent<SpriteComponent>();
		if (sprite.m_TextureHandle.IsValid())
			AssetManager::Release(sprite.m_TextureHandle);
	}

	void ComponentObserver::InitComponentObservers(entt::registry& reg, Scene* scene)
	{
		s_ActiveScene = scene;

		// on_construct
		reg.on_construct<QuadComponent>().connect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().connect<&OnCameraConstruct>();
		reg.on_construct<ScriptComponent>().connect<&OnScriptConstruct>();
		reg.on_construct<SpriteComponent>().connect<&OnSpriteConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().connect<&OnCameraDestruct>();
		reg.on_destroy<SpriteComponent>().connect<&OnSpriteDestruct>();
	}

	void ComponentObserver::DeinitConstructObservers(entt::registry& reg)
	{
		// on_construct
		reg.on_construct<QuadComponent>().disconnect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().disconnect<&OnCameraConstruct>();
		reg.on_construct<ScriptComponent>().disconnect<&OnScriptConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().disconnect<&OnCameraDestruct>();
	}

}