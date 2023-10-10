#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"

namespace Stimpi
{

	// Test entity
	Entity s_TestObj;

	// TODO: clean - don't create stuff here
	Scene::Scene()
	{
		m_SceneCamera = std::make_shared<Camera>(0.0f, 1280.0f, 0.0f, 720.0f);
		m_CameraController = std::make_shared<CameraController>(m_SceneCamera.get());

		m_DefaultShader.reset(Shader::CreateShader("shaders\/shader.shader"));

		s_TestObj = CreateEntity("TestObj");
		s_TestObj.AddComponent<QuadComponent>(glm::vec4{ 100.0f, 100.0f, 50.0f, 50.0f });
		s_TestObj.AddComponent<TextureComponent>("Picture1.jpg");

		class QuadController : public ScriptableEntity
		{
		public:
			void OnCreate()
			{

			}

			void OnDestroy()
			{

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
		// TODO: move to Scene::OnScenePlay
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs) 
				{
					if (!ncs.m_Instance)
					{
						ncs.m_Instance = ncs.InstantiateScript();
						ncs.m_Instance->m_Entity = Entity{ entity, this };
						ncs.m_Instance->OnCreate();
					}

					ncs.m_Instance->OnUpdate(ts);
				});
		}

		// Resize Camera
		//auto frameBuffer = Renderer2D::Instace()->GetFrameBuffer();
		//m_SceneCamera->Resize(0.0f, frameBuffer->GetWidth(), 0.0f, frameBuffer->GetHeight());

		// Movement control
		m_CameraController->Update(ts);

		// Scene rendering
		Stimpi::Renderer2D::Instace()->BeginScene(m_SceneCamera->GetOrthoCamera());

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