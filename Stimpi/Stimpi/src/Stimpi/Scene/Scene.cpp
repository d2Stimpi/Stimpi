#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"

namespace Stimpi
{

	// Test entity
	Entity s_TestObj;

	Scene::Scene()
	{
		// Hack?? Create 0 value Entity and never use it. Fixes check for valid Entity
		m_Registry.create();

		m_RuntimeState = RuntimeState::STOPPED;
		m_DefaultShader.reset(Shader::CreateShader("shaders\/shader.shader"));

		/* Test stuff below */
		m_SubTexture = std::make_shared<SubTexture>(ResourceManager::Instance()->LoadTexture("..\/assets\/sprite_sheets\/sonic-sprite-sheet.png"), glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 150.0f, 150.0f });

		s_TestObj = CreateEntity("ScriptedObj");
		s_TestObj.AddComponent<QuadComponent>(glm::vec4{ 100.0f, 100.0f, 50.0f, 50.0f });
		s_TestObj.AddComponent<TextureComponent>("Picture1.jpg");

		auto camera = std::make_shared<Camera>(0.0f, 1280.0f, 0.0f, 720.0f);
		s_TestObj.AddComponent<CameraComponent>(camera, true);

		class QuadController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{
				ST_CORE_INFO("QuadController OnCreate()");
			}

			void OnDestroy()
			{
				ST_CORE_INFO("QuadController OnDestroy()");
			}

			void OnUpdate(Timestep ts)
			{
				auto& quad = GetComponent<QuadComponent>();
				static float moveSpeed = 400.f;

				if (InputManager::IsKeyPressed(ST_KEY_T))
					quad.m_Y += moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_G))
					quad.m_Y -= moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_H))
					quad.m_X += moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_F))
					quad.m_X -= moveSpeed * ts;
			}
		};

		s_TestObj.AddComponent<NativeScriptComponent>().Bind<QuadController>();
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Update NativeScripts
		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
				{
					if (ncs.m_Instance)
					{
						ncs.m_Instance->OnUpdate(ts);
					}
				});
		}

		/* When not in Running state, use Editor sourced camera */
		if (m_RuntimeState == RuntimeState::STOPPED)
		{
			m_RenderCamera = m_SceneCamera;
		}

		// Scene rendering
		if (m_RenderCamera)
		{
			Stimpi::Renderer2D::Instace()->BeginScene(m_RenderCamera->GetOrthoCamera());

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

			Stimpi::Renderer2D::Instace()->Submit(glm::vec4{ 150.0f, 250.0f, 150.0f, 150.0f }, m_SubTexture.get(), m_DefaultShader.get());

			Stimpi::Renderer2D::Instace()->EndScene();
		}
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

	void Scene::RemoveEntity(Entity entity)
	{
		m_Registry.destroy(entity.GetHandle());
		m_Entities.erase(std::remove(std::begin(m_Entities), std::end(m_Entities), entity));
	}

	void Scene::SetCamera(Camera* camera)
	{
		m_SceneCamera = camera;
	}

	void Scene::OnScenePlay()
	{
		m_RuntimeState = RuntimeState::RUNNING;
		
		/* Create NativeScripts */
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
			{
				if (!ncs.m_Instance)
				{
					ncs.m_Instance = ncs.InstantiateScript();
					ncs.m_Instance->m_Entity = Entity{ entity, this };
					ncs.m_Instance->OnCreate();
				}
			});

		m_Registry.view<CameraComponent>().each([=](auto entity, auto& ncs)
			{
				if (ncs.m_IsMain)
				{
					m_RenderCamera = ncs.m_Camera.get();
				}
			});
	}

	void Scene::OnScenePause()
	{
		m_RuntimeState = RuntimeState::PAUSED;
	}

	void Scene::OnSceneStop()
	{
		m_RuntimeState = RuntimeState::STOPPED;

		/* Destroy NativeScripts */
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
			{
				if (ncs.m_Instance)
				{
					ncs.m_Instance->OnDestroy();
					ncs.DestroyScript(&ncs);
				}
			});

		// TODO: reload Scene to reset all Entities states
	}

}