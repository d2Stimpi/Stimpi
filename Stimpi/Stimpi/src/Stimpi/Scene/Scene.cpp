#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Log.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"
#include "Stimpi/Scene/Assets/AssetManager.h"

#include "Stimpi/Physics/ContactListener.h"
#include "Stimpi/Physics/Physics.h"

#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Core/Time.h"

#include "Stimpi/Scripting/ScriptEngine.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include "Stimpi/Graphics/Animation/AnimationSerializer.h"
// TODO: remove eventually
#define USE_TEST_STUFF false

namespace Stimpi
{
	struct SceneConfigPanel
	{
		// Debugging
		bool m_EnableDebug = false;
	};

	static SceneConfigPanel s_Config;

	void Scene::EnableDebugMode(bool enable)
	{
		s_Config.m_EnableDebug = enable;
	}

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

		// Test save
		/*Animation anim;
		AnimationSerializer serializer(&anim);
		serializer.Serialize("..\/assets\/animations\/test.anim");*/
		// Test load
		/*Animation* animLoad = Animation::Create("..\/assets\/animations\/test.anim");
		ST_CORE_INFO("");*/

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

		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			// Update Scripts
			UpdateScripts(ts);

			// Physics
			UpdatePhysicsSimulation(ts);

			// Update components that depend on timestep
			UpdateComponents(ts);
		}
		else
		{
			// Ensure rendering of AnimSprite frame 0
			UpdateComponents(0);
		}

		/* When not in Running state, use Editor sourced camera */
		if (m_RuntimeState == RuntimeState::STOPPED)
		{
			m_RenderCamera = m_SceneCamera;
		}

		// Scene Rendering
		if (m_RenderCamera)
		{
			GraphicsConfig graphicsConfig = Project::GetGraphicsConfig();

			/** TODO: optimize
			 * When in "Editor mode" we can check and sort by layers each frame.
			 * In "Runtime mode" entities can be sorted once when created and grouped by layer.
			 * Each time an entity would change the SortingGroup the containers can be updated only then.
			 * Or just simply do it for both modes :P
			 */

			auto& sortingLayers = Project::GetSortingLayers();
			for (auto& layer : sortingLayers)
			{
				std::vector<Entity> layerGroup;
				auto group = m_Registry.group<SortingGroupComponent>();
				
				// Filter by layer
				for (auto e : group)
				{
					Entity entity = { e, this };
					if (entity.GetComponent<SortingGroupComponent>().m_SortingLayerName == layer->m_Name)
						layerGroup.emplace_back(entity);
				}

				// Order by layer ID
				std::sort(layerGroup.begin(), layerGroup.end(), [&graphicsConfig, this](auto a, auto b)
					{
						auto idA = a.GetComponent<SortingGroupComponent>().m_OrderInLayer;
						auto idB = b.GetComponent<SortingGroupComponent>().m_OrderInLayer;

						if (idA == idB && graphicsConfig.m_RenderingOrderAxis != RenderingOrderAxis::None)
						{
							return CompareByAxis(a, b);
						}

						return idA < idB;
					});

				// Pass filtered and sorted entities for rendering
				if (!layerGroup.empty())
				{
					Stimpi::Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
					SubmitForRendering(layerGroup);
					Stimpi::Renderer2D::Instance()->EndScene();
				}
			}

			// Render entities that don't use SortingGroup component
			Stimpi::Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());

			std::vector<Entity> unordered;
			for (auto entity : m_Entities)
			{
				if (!entity.HasComponent<SortingGroupComponent>())
					unordered.emplace_back(entity);
			}

			if (graphicsConfig.m_RenderingOrderAxis != RenderingOrderAxis::None)
			{
				std::sort(unordered.begin(), unordered.end(), [&graphicsConfig, this](auto a, auto b)
					{
						return CompareByAxis(a, b);
					});
			}

			SubmitForRendering(unordered);

			Stimpi::Renderer2D::Instance()->EndScene();
		}
		
		if (s_Config.m_EnableDebug)
			OnDebugUpdate(ts);
	}

	void Scene::SubmitForRendering(std::vector<Entity>& entities)
	{
		for (auto entity : entities)
		{
			if (entity.HasComponent<QuadComponent>())
			{
				auto& quad = entity.GetComponent<QuadComponent>();
				if (entity.HasComponent<SpriteComponent>())
				{
					auto& sprite = entity.GetComponent<SpriteComponent>();
					if (sprite.TextureLoaded())
					{
						if (sprite.m_Enable)
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite, m_DefaultShader.get());
						else
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
					}
					else
					{
						Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
					}
				}

				// Draw AnimatedSprite if available (can overlap with Sprite)
				if (entity.HasComponent<AnimatedSpriteComponent>())
				{
					auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
					if (anim.Loaded())
					{
						Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, anim, m_DefaultShader.get());
					}
				}

				// Draw debug RigidBody Collider outline
				if (Physics::ShowColliderOutlineEnabled())
				{
					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
						glm::vec3 outlineColor(0.80f, 0.3f, 0.2f);
						glm::vec3 outlinePos(quad.Center() + bc2d.m_Offset, 0.0f);
						glm::vec2 outlineSize(bc2d.m_Size.x * quad.m_Size.x * 2.0f, bc2d.m_Size.y * quad.m_Size.y * 2.0f);

						if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::BOX)
						{
							Stimpi::Renderer2D::Instance()->SubmitSquare(outlinePos, outlineSize, quad.m_Rotation, outlineColor);
						}
						else if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::CIRLCE)
						{
							glm::vec2 circleOutlineSize(bc2d.m_Size.x * quad.m_Size.x * 2.0f, bc2d.m_Size.x * quad.m_Size.x * 2.0f);
							Stimpi::Renderer2D::Instance()->SubmitCircle(outlinePos, circleOutlineSize, outlineColor, 0.06f, 0.0f);
						}
					}
				}
			}
			else if (entity.HasComponent<CircleComponent>())
			{
				auto& circle = entity.GetComponent<CircleComponent>();
				if (entity.HasComponent<SpriteComponent>())
				{
					auto& sprite = entity.GetComponent<SpriteComponent>();
					if (sprite.TextureLoaded() && sprite.m_Enable)
					{
						Renderer2D::Instance()->Submit(circle.m_Position, circle.m_Size, circle.m_Rotation, sprite, m_DefaultShader.get());
					}
					else
					{
						Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
					}
				}
				else
				{
					Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
				}

				// Draw debug RigidBody Collider outline
				if (Physics::ShowColliderOutlineEnabled())
				{
					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
						glm::vec3 outlineColor(0.80f, 0.3f, 0.2f);
						glm::vec3 outlinePos(circle.Center() + bc2d.m_Offset, 0.0f);
						glm::vec2 outlineSize(bc2d.m_Size.x * circle.m_Size.x * 2.0f, bc2d.m_Size.y * circle.m_Size.y * 2.0f);

						if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::BOX)
						{
							Stimpi::Renderer2D::Instance()->SubmitSquare(outlinePos, outlineSize, circle.m_Rotation, outlineColor);
						}
						else if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::CIRLCE)
						{
							glm::vec2 circleOutlineSize(bc2d.m_Size.x * circle.m_Size.x * 2.0f, bc2d.m_Size.x * circle.m_Size.x * 2.0f);
							Stimpi::Renderer2D::Instance()->SubmitCircle(outlinePos, circleOutlineSize, outlineColor, 0.06f, 0.0f);
						}
					}
				}
			}
		}
	}

	bool Scene::CompareByAxis(Entity a, Entity b)
	{
		if ((a.HasComponent<QuadComponent>() || a.HasComponent<CircleComponent>()) &&
			(b.HasComponent<QuadComponent>() || b.HasComponent<CircleComponent>()))
		{
			glm::vec3 posA = a.HasComponent<QuadComponent>() ? a.GetComponent<QuadComponent>().m_Position :
				a.GetComponent<CircleComponent>().m_Position;
			glm::vec3 posB = b.HasComponent<QuadComponent>() ? b.GetComponent<QuadComponent>().m_Position :
				b.GetComponent<CircleComponent>().m_Position;

			auto graphicsConfig = Project::GetGraphicsConfig();
			if (graphicsConfig.m_RenderingOrderAxis == RenderingOrderAxis::X_AXIS)
				return posA.x < posB.x;
			if (graphicsConfig.m_RenderingOrderAxis == RenderingOrderAxis::Y_AXIS)
				return posA.y < posB.y;
			if (graphicsConfig.m_RenderingOrderAxis == RenderingOrderAxis::Z_AXIS)
				return posA.z < posB.z;

			// Default sorting
			return posA.z < posB.z;
		}

		return false;
	}

	void Scene::OnSceneStep()
	{
		Timestep ts = Time::Instance()->GetFrameTime();

		UpdateComponentDependacies(ts);

		// Update Scripts
		UpdateScripts(ts);

		// Physics
		UpdatePhysicsSimulation(ts);

		// Update components that depend on timestep
		UpdateComponents(ts);
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
		m_RenderCamera = camera;
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

		Physics::ClearActiveCollisions();
	}

	void Scene::OnSortingLayerRemove(const std::string layerName)
	{
		m_Registry.view<SortingGroupComponent>().each([&layerName](auto entity, auto& group)
			{
				if (group.m_SortingLayerName == layerName)
				{
					group.m_SortingLayerName = Project::GetDefaultSortingLayerName();
				}
			});
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

		m_Registry.view<CircleComponent>().each([this, &picked, x, y](auto entity, auto& circle)
			{
				if (SceneUtils::IsPointInCircle({ x, y }, circle.Center(), circle.MaxRadius() / 2.0f))
				{
					picked = Entity(entity, this);
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

	void Scene::UpdateComponents(Timestep ts)
	{
		m_Registry.view<AnimatedSpriteComponent>().each([&ts](auto e, AnimatedSpriteComponent anim)
			{
				if (anim.m_AnimSprite)
				{
					anim.m_AnimSprite->Update(ts);
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
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& ncs)
			{
				if (ncs.m_Instance)
				{
					ncs.m_Instance->OnUpdate(ts);
				}
			});

		ScriptEngine::OnSceneUpdate(ts);
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
				if (entity.HasComponent<QuadComponent>() || entity.HasComponent<CircleComponent>())
				{
					glm::vec2 center(0.0f, 0.0f);
					glm::vec2 size(0.0f, 0.0f);
					float rotation = 0.0f;

					if (entity.HasComponent<QuadComponent>())
					{
						auto& quad = entity.GetComponent<QuadComponent>();
						center = quad.Center();
						size = quad.m_Size;
						rotation = quad.m_Rotation;
					}
					else if (entity.HasComponent<CircleComponent>())
					{
						auto& cicle = entity.GetComponent<CircleComponent>();
						center = cicle.Center();
						size = cicle.m_Size;
						rotation = cicle.m_Rotation;
					}

					b2BodyDef bodyDef;
					bodyDef.type = Rigidbody2DTypeToBox2DType(rb2d.m_Type);
					bodyDef.position.Set(center.x, center.y);
					bodyDef.angle = rotation;
					bodyDef.userData.pointer = (uint32_t)entity;

					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
						b2FixtureDef fixtureDef;
						b2PolygonShape boxShape;
						b2CircleShape circleShape;

						if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::BOX)
						{
							boxShape.SetAsBox(bc2d.m_Size.x * size.x, bc2d.m_Size.y * size.y);
							fixtureDef.shape = &boxShape;
						}
						else if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::CIRLCE)
						{
							circleShape.m_radius = bc2d.m_Size.x * size.x;
							fixtureDef.shape = &circleShape;
						}

						fixtureDef.density = bc2d.m_Density;
						fixtureDef.friction = bc2d.m_Friction;
						fixtureDef.restitution = bc2d.m_Restitution;
						fixtureDef.restitutionThreshold = bc2d.m_RestitutionThreshold;

						bodyDef.position.Set(center.x, center.y);
						b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
						body->SetFixedRotation(rb2d.m_FixedRotation);
						body->CreateFixture(&fixtureDef);
						rb2d.m_RuntimeBody = body;
					}
					else
					{
						b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
						body->SetFixedRotation(rb2d.m_FixedRotation);
						rb2d.m_RuntimeBody = body;
					}
				}
			});

		m_PhysicsWorld->SetContactListener(m_ContactListener.get());
	}

	void Scene::UpdatePhysicsSimulation(Timestep ts)
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
					quad.m_Position.x = position.x;
					quad.m_Position.y = position.y;
					quad.m_Rotation = body->GetAngle();

					// Updated quad position by Collider offset
					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
						quad.m_Position.x -= bc2d.m_Offset.x;
						quad.m_Position.y -= bc2d.m_Offset.y;
					}
				}

				if (entity.HasComponent<CircleComponent>())
				{
					auto& circle = entity.GetComponent<CircleComponent>();

					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					const auto& position = body->GetPosition();
					circle.m_Position.x = position.x;
					circle.m_Position.y = position.y;
					circle.m_Rotation = body->GetAngle();

					// Updated quad position by Collider offset
					if (entity.HasComponent<BoxCollider2DComponent>())
					{
						auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
						circle.m_Position.x -= bc2d.m_Offset.x;
						circle.m_Position.y -= bc2d.m_Offset.y;
					}
				}
			});
	}

	void Scene::DeinitializePhysics()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	bool Scene::ProcessPhysicsEvent(PhysicsEvent* event)
	{
		// Check for correct Scene runtime state first
		if (m_RuntimeState == RuntimeState::STOPPED)
		{
			return false;
		}

		Collision collisionData = event->GetCollisionData();
		Entity owner = { (entt::entity)collisionData.m_Owner, this };
		
		if (event->GetType() == PhysicsEventType::COLLISION_BEGIN)
		{
			// Register current active Collisions
			Physics::AddActiveCollision(new Collision(collisionData));
		}
		else if (event->GetType() == PhysicsEventType::COLLISION_END)
		{
			// Remove from active Collisions list
			Physics::RemoveActiveCollision(&collisionData);
		}
		else if (event->GetType() == PhysicsEventType::COLLISION_PRESOLVE)
		{
			
		}
		else if (event->GetType() == PhysicsEventType::COLLISION_POSTSOLVE)
		{
			// Remove from active Collisions list
			Physics::UpdateActiveCollision(&collisionData);
		}

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

			if (event->GetType() == PhysicsEventType::COLLISION_PRESOLVE)
			{
				instance->InvokeOnCollisionPreSolve(event->GetCollisionData());
				return true;
			}

			if (event->GetType() == PhysicsEventType::COLLISION_POSTSOLVE)
			{
				instance->InvokeOnCollisionPostSolve(event->GetCollisionData());
				return true;
			}
		}

		return false;
	}

	void Scene::OnDebugUpdate(Timestep ts)
	{
		auto activeCollisions = Physics::GetActiveCollisions();

		// Debug render - Collision contact points
		if (m_RenderCamera)
		{
			Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
		
			if (Physics::ShowCollisionsContactPointsEnabled())
			{
				for (auto collision : activeCollisions)
				{
					for (Contact& contact : collision->m_Contacts)
					{
						Renderer2D::Instance()->SubmitCircle({ contact.m_Point.x, contact.m_Point.y, 0.0f }, { 1.0f, 1.0f }, {}, 1.0f, 0.005f);
					}
				}
			}

			Renderer2D::Instance()->EndScene();
		}
	}

}