#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"
#include "Stimpi/Physics/ContactListener.h"

#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"

#include "Stimpi/Scripting/ScriptEngine.h"

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

		// Clear script instances - they are created in OnScriptConstruct
		ScriptEngine::ClearScriptInstances();

		m_RuntimeState = RuntimeState::STOPPED;
		m_DefaultShader.reset(Shader::CreateShader("..\/assets\/shaders\/shader.shader"));
		m_DefaultSolidColorShader.reset(Shader::CreateShader("..\/assets\/shaders\/solidcolor.shader"));

		// Collision listener for Box2D
		m_ContactListener = std::make_unique<ContactListener>();
		m_ContactListener->SetContext(this);

#if USE_TEST_STUFF
		/* Test stuff below */
		m_SubTexture = std::make_shared<SubTexture>(ResourceManager::Instance()->LoadTexture("..\/assets\/sprite_sheets\/sonic-sprite-sheet.png"), glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 150.0f, 150.0f });
		m_TestTexture = ResourceManager::Instance()->LoadTexture("..\/assets\/textures\/Picture1.jpg");

		s_TestObj = CreateEntity("ScriptedObj");
		s_TestObj.AddComponent<QuadComponent>(glm::vec4{ 100.0f, 100.0f, 50.0f, 50.0f });
		s_TestObj.AddComponent<SpriteComponent>("..\/assets\/textures\/Picture1.jpg");

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

		// Update Scripts
		UpdateScripts(ts);

		// Physics
		UpdatePhysicsSimulation(ts);

		/* When not in Running state, use Editor sourced camera */
		if (m_RuntimeState == RuntimeState::STOPPED)
		{
			m_RenderCamera = m_SceneCamera;
		}

		// Scene Rendering
		if (m_RenderCamera)
		{
			Stimpi::Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());

			for (auto entity : m_Entities)
			{
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();
					if (entity.HasComponent<SpriteComponent>())
					{
						auto& sprite = entity.GetComponent<SpriteComponent>();
						if (sprite.m_Texture)
						{
							if (sprite.m_Enable)
								Renderer2D::Instance()->Submit(quad, quad.m_Rotation, sprite.m_Texture, m_DefaultShader.get());
							else
								Renderer2D::Instance()->Submit(quad, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
						}
						else
						{
							Renderer2D::Instance()->Submit(quad, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
						}
					}
				}
			}

			m_Registry.view<CircleComponent>().each([=](auto entity, CircleComponent& circle)
				{
					glm::vec3 position = { circle.m_Position.x, circle.m_Position.y, 1.0f };
					Renderer2D::Instance()->DrawCircle(position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
				});

#if USE_TEST_STUFF
			//Stimpi::Renderer2D::Instace()->Submit(glm::vec4{ 150.0f, 250.0f, 150.0f, 150.0f }, m_SubTexture.get(), m_DefaultShader.get());
			Stimpi::Renderer2D::Instance()->Submit(glm::vec3{ 225.0f, 220.0f, 0.0f }, glm::vec2{ 50.0f, 40.0f }, 15.0f, m_TestTexture, m_DefaultShader.get());
			Stimpi::Renderer2D::Instance()->Submit(glm::vec4{ 350.0f, 250.0f, 150.0f, 150.0f }, 45.0f, m_TestTexture, m_DefaultShader.get());
			Stimpi::Renderer2D::Instance()->DrawCircle(glm::vec3{ 50.0f, 40.0f, 0.0f }, glm::vec2{ 50.0f, 50.0f }, glm::vec3{ 0.35f, 0.85f, 0.2f }, 0.02f, 0.005f);
			Stimpi::Renderer2D::Instance()->DrawCircle(glm::vec3{ 80.0f, 60.0f, 0.0f }, glm::vec2{ 50.0f, 50.0f }, glm::vec3{ 0.35f, 0.85f, 0.2f }, 0.5f, 0.005f);
			Stimpi::Renderer2D::Instance()->DrawCircle(glm::vec3{ 120.0f, 80.0f, 0.0f }, glm::vec2{ 50.0f, 50.0f }, glm::vec3{ 0.35f, 0.85f, 0.2f }, 1.0f, 0.5f);
#endif
			Stimpi::Renderer2D::Instance()->EndScene();
		}
	}

	void Scene::OnEvent(Event* event)
	{
		EventDispatcher<PhysicsEvent> eventDispatcher;
		eventDispatcher.Dispatch(event, [&](PhysicsEvent* e) -> bool {
			return ProcessPhysicsEvent(e);;
		});
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

	Stimpi::Entity Scene::FindentityByName(std::string_view name)
	{
		auto view = m_Registry.view<TagComponent>();
		for (auto entity : view)
		{
			const TagComponent& tag = view.get<TagComponent>(entity);
			if (tag.m_Tag == name)
				return Entity{ entity, this };
		}
		return {};
	}

	void Scene::RemoveEntity(Entity entity)
	{
		m_Registry.destroy(entity.GetHandle());
		m_Entities.erase(std::remove(std::begin(m_Entities), std::end(m_Entities), entity));
	}

	Stimpi::Entity Scene::CopyEntity(const Entity entity)
	{
		Entity newEntity = { m_Registry.create(), this };
		auto dst = newEntity.GetHandle();
		for (auto [id, storage] : m_Registry.storage())
		{
			auto src = entity.GetHandle();
			if (storage.contains(src))
			{
				storage.push(dst, storage.value(src));
			}
		}

		m_Entities.push_back(newEntity);
		return newEntity;
	}

	void Scene::SetCamera(Camera* camera)
	{
		m_SceneCamera = camera;
	}

	void Scene::OnScenePlay()
	{
		InitializeScripts();
		InitializePhysics();

		m_Registry.view<CameraComponent>().each([=](auto entity, auto& ncs)
			{
				if (ncs.m_IsMain)
				{
					m_RenderCamera = ncs.m_Camera.get();
				}
			});

		// Update Scene state last
		m_RuntimeState = RuntimeState::RUNNING;
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
		DeinitializeScritps();
		DeinitializePhysics();

		// Update Scene state last
		m_RuntimeState = RuntimeState::STOPPED;
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

	/* ======== Scripting ======== */

	void Scene::InitializeScripts()
	{
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

		// Create C# scripts
		ScriptEngine::OnScenePlay();
	}

	void Scene::UpdateScripts(Timestep ts)
	{
		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
				{
					if (ncs.m_Instance)
					{
						ncs.m_Instance->OnUpdate(ts);
					}
				});

			ScriptEngine::OnSceneUpdate(ts);
		}
	}

	void Scene::DeinitializeScritps()
	{
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
			{
				if (ncs.m_Instance)
				{
					ncs.m_Instance->OnDestroy();
					ncs.DestroyScript(&ncs);
				}
			});
		
		ScriptEngine::OnSceneStop();
	}

	/* ======== Physics ======== */

	void Scene::InitializePhysics()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });
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
					bodyDef.userData.pointer = (uint32_t)entity;

					b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
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

		m_PhysicsWorld->SetContactListener(m_ContactListener.get());
	}

	void Scene::UpdatePhysicsSimulation(Timestep ts)
	{
		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

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
	}

	void Scene::DeinitializePhysics()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	bool Scene::ProcessPhysicsEvent(PhysicsEvent* event)
	{
		// Check for correct Scene runtime state first
		if (m_RuntimeState != RuntimeState::RUNNING)
		{
			return false;
		}

		Collision collisionData = event->GetCollisionData();
		Entity owner = { (entt::entity)collisionData.m_Owner, this };
		
		if (owner.HasComponent<ScriptComponent>())
		{
			auto instance = ScriptEngine::GetScriptInstance(owner);
			ST_CORE_ASSERT_MSG(!instance, "Processing physics event, but no Script instance!");

			if (event->GetType() == PhysicsEventType::COLLISION_BEGIN)
			{
				instance->InvokeOnCollisionBegin(event->GetCollisionData());
				return true;
			}

			if (event->GetType() == PhysicsEventType::COLLISION_END)
			{
				instance->InvokeOnCollisionEnd(event->GetCollisionData());
				return true;
			}
		}

		return false;
	}

}