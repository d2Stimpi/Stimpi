#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Graphics/Renderer2D.h"

namespace Stimpi
{
	Scene::Scene()
	{
		auto entity = CreateEntity();
		entity.AddComponent<TagComponent>("Entity");
		entity.AddComponent<QuadComponent>(glm::vec4(0.0f, 0.0f, 50.0f, 40.0f));

		auto view = m_Registry.view<TagComponent>();
		for (auto item : view)
		{
			auto tag = view.get<TagComponent>(item);
		}
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Render all Square components
		auto view = m_Registry.view<QuadComponent>();
		for (auto entity : view)
		{
			auto quad = view.get<QuadComponent>(entity);
			Renderer2D::Instace()->Submit(quad);
		}
	}

	Entity Scene::CreateEntity()
	{
		Entity entity = { m_Registry.create(), this };
		return entity;
	}
}