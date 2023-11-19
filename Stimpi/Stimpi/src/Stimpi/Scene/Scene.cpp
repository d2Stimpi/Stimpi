#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

// TODO: remove eventually
#define USE_TEST_STUFF false

namespace Stimpi
{
	static b2BodyType Rigidbody2DTypeToBox2DType(RigidBody2DComponent::BodyType type)
	{
		switch (type)
		{
		    case RigidBody2DComponent::BodyType::STATIC:	return b2_staticBody;
		    case RigidBody2DComponent::BodyType::DYNAMIC:	return b2_dynamicBody;
		    case RigidBody2DComponent::BodyType::KINEMATIC: return b2_kinematicBody;
		}

		ST_CORE_ASSERT(false, "Unknown physics body type");
		return b2_staticBody;
	}


#if USE_TEST_STUFF
	// Test entity
	Entity s_TestObj;
#endif

	Scene::Scene()
	{
		// Workaround - Create 0 value Entity and never use it. Fixes check for valid Entity
		m_Registry.create();
		ComponentObserver::InitOnConstructObservers(m_Registry, this);

		m_RuntimeState = RuntimeState::STOPPED;
		m_DefaultShader.reset(Shader::CreateShader("..\/assets\/shaders\/shader.shader"));

#if USE_TEST_STUFF
		/* Test stuff below */
		m_SubTexture = std::make_shared<SubTexture>(ResourceManager::Instance()->LoadTexture("..\/assets\/sprite_sheets\/sonic-sprite-sheet.png"), glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 150.0f, 150.0f });
		m_TestTexture = ResourceManager::Instance()->LoadTexture("..\/assets\/textures\/Picture1.jpg");

		s_TestObj = CreateEntity("ScriptedObj");
		s_TestObj.AddComponent<QuadComponent>(glm::vec4{ 100.0f, 100.0f, 50.0f, 50.0f });
		s_TestObj.AddComponent<TextureComponent>("..\/assets\/textures\/Picture1.jpg");

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
					quad.m_Position.y += moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_G))
					quad.m_Position.y -= moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_H))
					quad.m_Position.x += moveSpeed * ts;
				if (InputManager::IsKeyPressed(ST_KEY_F))
					quad.m_Position.x -= moveSpeed * ts;
			}
		};

		s_TestObj.AddComponent<NativeScriptComponent>().Bind<QuadController>();
#endif
	}

	Scene::~Scene()
	{
		ComponentObserver::DeinitOnConstructObservers(m_Registry);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		UpdateComponentDependacies(ts);

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

		// Physics
		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsDWorld->Step(ts, velocityIterations, positionIterations);

			m_Registry.view<RigidBody2DComponent>().each([=](auto e, RigidBody2DComponent& rb2d)
				{
					Entity entity = { e, this };
					if (entity.HasComponent<QuadComponent>())
					{
						auto& quad = entity.GetComponent<QuadComponent>();

						b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
						const auto& position = body->GetPosition();
						quad.m_Position.x = position.x - quad.HalfWidth();
						quad.m_Position.y = position.y - quad.HalfHeight();
						quad.m_Rotation = body->GetAngle();
					}
				});
		}

		/* When not in Running state, use Editor sourced camera */
		if (m_RuntimeState == RuntimeState::STOPPED)
		{
			m_RenderCamera = m_SceneCamera;
		}

		// Scene Rendering
		if (m_RenderCamera)
		{
			Stimpi::Renderer2D::Instace()->BeginScene(m_RenderCamera->GetOrthoCamera());

			for (auto entity : m_Entities)
			{
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();
					if (entity.HasComponent<TextureComponent>())
					{
						auto& texture = entity.GetComponent<TextureComponent>();
						if (texture)
							Renderer2D::Instace()->Submit(quad, quad.m_Rotation, texture, m_DefaultShader.get());
					}
				}
			}
#if USE_TEST_STUFF
			//Stimpi::Renderer2D::Instace()->Submit(glm::vec4{ 150.0f, 250.0f, 150.0f, 150.0f }, m_SubTexture.get(), m_DefaultShader.get());
			Stimpi::Renderer2D::Instace()->Submit(glm::vec3{ 225.0f, 220.0f, 0.0f }, glm::vec2{ 50.0f, 40.0f }, 15.0f, m_TestTexture, m_DefaultShader.get());
			Stimpi::Renderer2D::Instace()->Submit(glm::vec4{ 350.0f, 250.0f, 150.0f, 150.0f }, 45.0f, m_TestTexture, m_DefaultShader.get());
#endif
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

	Stimpi::Entity Scene::GetEntityByHandle(entt::entity handle)
	{
		return Entity(handle, this);
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
		
		// Create NativeScripts
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

		// Init Physics
		m_PhysicsDWorld = new b2World({0.0f, -9.8f});
		m_Registry.view<RigidBody2DComponent>().each([=](auto e, auto& rb2d)
			{
				Entity entity = { e, this };
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();

					b2BodyDef bodyDef;
					bodyDef.type = Rigidbody2DTypeToBox2DType(rb2d.m_Type);
					bodyDef.position.Set(quad.Center().x, quad.Center().y);
					bodyDef.angle = quad.m_Rotation;

					b2Body* body = m_PhysicsDWorld->CreateBody(&bodyDef);
					body->SetFixedRotation(rb2d.m_FixedRotation);
					rb2d.m_RuntimeBody = body;

					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

						b2PolygonShape boxShape;
						boxShape.SetAsBox(bc2d.m_Size.x * quad.m_Size.x, bc2d.m_Size.y * quad.m_Size.y);

						b2FixtureDef fixtureDef;
						fixtureDef.shape = &boxShape;
						fixtureDef.density = bc2d.m_Density;
						fixtureDef.friction = bc2d.m_Friction;
						fixtureDef.restitution = bc2d.m_Restitution;
						fixtureDef.restitutionThreshold = bc2d.m_RestitutionThreshold;
						body->CreateFixture(&fixtureDef);
					}
				}
			});
	}

	void Scene::OnScenePause()
	{
		m_RuntimeState = RuntimeState::PAUSED;
	}

	void Scene::OnSceneResume()
	{
		m_RuntimeState = RuntimeState::RUNNING;
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


		// De-Init Physics
		delete m_PhysicsDWorld;
		m_PhysicsDWorld = nullptr;
	}

	Stimpi::Entity Scene::MousePickEntity(float x, float y)
	{
		Entity picked = {};

		m_Registry.view<QuadComponent>().each([this, &picked, x, y](auto entity, auto& quad)
			{
				if (quad.m_PickEnabled)
				{
					if (SceneUtils::IsPointInRotatedSquare({ x, y }, quad.Center(), quad.m_Size, quad.m_Rotation))
					{
						picked = Entity(entity, this);
					}
				}
			});

		return picked;
	}

	void Scene::UpdateComponentDependacies(Timestep ts)
	{
		m_Registry.view<CameraComponent>().each([this](auto e, CameraComponent camera)
			{
				Entity entitiy = { e, this };
				if (entitiy.HasComponent<QuadComponent>())
				{
					auto& quad = entitiy.GetComponent<QuadComponent>();
					auto camPos = camera.m_Camera->GetPosition();
					camera.m_Camera->SetPosition({ quad.m_Position.x , quad.m_Position.y, camPos.z });
				}
			});
	}

}