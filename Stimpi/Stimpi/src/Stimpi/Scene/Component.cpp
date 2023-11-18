#include "stpch.h"
#include "Stimpi/Scene/Component.h"

namespace Stimpi
{
	Scene* s_ActiveScene = nullptr;

	static void OnQuadConstruct(entt::registry& reg, entt::entity ent)
	{

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

	void ComponentObserver::InitOnConstructObservers(entt::registry& reg, Scene* scene)
	{
		s_ActiveScene = scene;

		// on_construct
		reg.on_construct<QuadComponent>().connect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().connect<&OnCameraConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().connect<&OnCameraDestruct>();
	}

	void ComponentObserver::DeinitOnConstructObservers(entt::registry& reg)
	{
		s_ActiveScene = nullptr;

		// on_construct
		reg.on_construct<QuadComponent>().disconnect<&OnQuadConstruct>();
		reg.on_construct<CameraComponent>().disconnect<&OnCameraConstruct>();

		// on_destroy
		reg.on_destroy<CameraComponent>().disconnect<&OnCameraDestruct>();
	}

}