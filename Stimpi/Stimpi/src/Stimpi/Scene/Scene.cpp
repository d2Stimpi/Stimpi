#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"

namespace Stimpi
{
	// TODO: clean - don't create stuff here
	Scene::Scene()
	{
		m_SceneCamera = std::make_shared<OrthoCamera>(0.0f, 1280.0f, 0.0f, 720.0f);
		m_DefaultShader.reset(Stimpi::Shader::CreateShader("shaders\/shader.shader"));
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Movement control
		static float moveSpeed = 400.f;

		if (InputManager::IsKeyPressed(ST_KEY_W))
			m_SceneCamera->Translate({ 0.0f, moveSpeed * ts, 0.0f });
		if (InputManager::IsKeyPressed(ST_KEY_S))
			m_SceneCamera->Translate({ 0.0f, -moveSpeed * ts, 0.0f });
		if (InputManager::IsKeyPressed(ST_KEY_D))
			m_SceneCamera->Translate({ moveSpeed * ts, 0.0f, 0.0f });
		if (InputManager::IsKeyPressed(ST_KEY_A))
			m_SceneCamera->Translate({ -moveSpeed * ts, 0.0f, 0.0f });

		// Scene rendering
		Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera.get());

		for (auto entity : m_Entities)
		{
			if (entity.HasComponent<QuadComponent>())
			{
				auto quad = entity.GetComponent<QuadComponent>();
				if (entity.HasComponent<TextureComponent>())
				{
					auto texture = entity.GetComponent<TextureComponent>();
					if (texture)
						Renderer2D::Instace()->Submit(quad, texture, m_DefaultShader.get());
				}
			}
		}

		Stimpi::Renderer2D::Instace()->EndScene();
	}

	void Scene::OnEvent(Event* event)
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

		m_Entities.push_back(entity);
		return entity;
	}
}