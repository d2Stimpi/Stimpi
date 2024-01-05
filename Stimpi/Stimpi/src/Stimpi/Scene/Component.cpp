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
			ScriptComponent component = entity.GetComponent<ScriptComponent>();
			if (!component.m_ScriptName.empty())
			{
				//TODO: Check if already created
				ScriptEngine::OnScriptComponentAdd(component.m_ScriptName, entity);
			}
		}
		
	}

	void ComponentObserver::InitOnConstructObservers(entt::registry& reg, Scene* scene)
	{
		s_ActiveScene = scene;

		// on_construct
		reg.on_construct<QuadComponent>().connect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().connect<&OnCameraConstruct>();
		reg.on_construct<ScriptComponent>().connect<&OnScriptConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().connect<&OnCameraDestruct>();
	}

	void ComponentObserver::DeinitOnConstructObservers(entt::registry& reg)
	{
		// on_construct
		reg.on_construct<QuadComponent>().disconnect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().disconnect<&OnCameraConstruct>();
		reg.on_construct<ScriptComponent>().disconnect<&OnScriptConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().disconnect<&OnCameraDestruct>();
	}

}