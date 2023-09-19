#include "Scene.h"

#include "Entity.h"

namespace Stimpi
{

	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate()
	{

	}

	Entity Scene::CreateEntity()
	{
		Entity entity = { m_Registry.create(), this };
		return entity;
	}
}