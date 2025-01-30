#include "stpch.h"
#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Log.h"

#include "Stimpi/Asset/ShaderImporter.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/EntityHierarchy.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Stimpi/Physics/ContactListener.h"
#include "Stimpi/Physics/Physics.h"

#include "Stimpi/Graphics/ShaderRegistry.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Core/Time.h"
#include "Stimpi/Debug/Statistics.h"

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
		ST_PROFILE_FUNCTION();

		// Workaround - Create 0 value Entity and never use it. Fixes check for valid Entity
		m_Registry.create();
		ComponentObserver::InitComponentObservers(m_Registry, this);

		// Clear script instances - they are created in OnScriptConstruct
		ScriptEngine::ClearScriptInstances();

		m_RuntimeState = RuntimeState::STOPPED;
		m_DefaultShader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/shader.shader");
		m_DefaultSolidColorShader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/solidcolor.shader");

		// Prepare Custom Shaders for usage, create required VAOs and BOs
		auto& customShaders = ShaderRegistry::GetShaderNames();
		for (auto& shader : customShaders)
		{
			AssetHandle handle = ShaderRegistry::GetShaderHandle(shader);
			Renderer2D::Instance()->RegisterShader(handle);
		}

		// Collision listener for Box2D
		m_ContactListener = std::make_unique<ContactListener>();
		m_ContactListener->SetContext(this);

#if USE_TEST_STUFF
		/* Test stuff below */

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
		ST_PROFILE_FUNCTION();

		ComponentObserver::DeinitConstructObservers(m_Registry);
	}

	void Scene::OnUpdate(Timestep ts)
	{
		ST_PROFILE_FUNCTION();

		if (m_RuntimeState == RuntimeState::RUNNING)
		{
			// Physics
			UpdatePhysicsSimulation(ts);

			// Update Scripts
			UpdateScripts(ts);

			// Update components that depend on timestep
			UpdateComponents(ts);
		}
		else
		{
			// Ensure rendering of AnimSprite frame 0
			UpdateComponents(0);
		}

		UpdateComponentDependacies(ts);

		// Scene Rendering
		if (m_RenderCamera)
		{
			ST_PROFILE_SCOPE("SubmitForRendering");
			Clock::Begin();

			SubmitForRendering(m_Entities);

			Statistics::SetRenderingTime(Clock::Stop());
		}
		
		// Debug render pass

		if (Physics::ShowColliderOutlineEnabled())
		{
			RenderDebugData();
		}

		if (s_Config.m_EnableDebug)
			OnDebugUpdate(ts);
	}

	void Scene::SubmitForRendering(std::vector<Entity>& entities)
	{
		auto view = m_Registry.view<DefaultGroupComponent, QuadComponent>();
		view.each(
			[](DefaultGroupComponent& def, QuadComponent& quad) {
				// Note: Possible issue if entity count exceeds 10k or position values are too high which is less likely.
				//	Big number for OrderInLayer may cause precision comparison issues. TODO: use precise float comparison.
				//	Layer count range [1...10].
				//	Consider using a variable value that depends on the entity count.
				def.m_Position = quad.m_Position + 100000.0f * def.m_LayerIndex + 1000000.0f * def.m_OrderInLayer;
			});

		auto axis = Project::GetGraphicsConfig().m_RenderingOrderAxis;
		if (axis == RenderingOrderAxis::X_AXIS)
		{
			m_Registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.x > rhs.m_Position.x;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}
		else if (axis == RenderingOrderAxis::Y_AXIS)
		{
			m_Registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.y > rhs.m_Position.y;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}
		else if (axis == RenderingOrderAxis::Z_AXIS)
		{
			m_Registry.sort<DefaultGroupComponent>([&](const auto& lhs, const auto& rhs)
				{
					if (lhs.m_LayerIndex == rhs.m_LayerIndex)
						return lhs.m_Position.z > rhs.m_Position.z;
					return lhs.m_LayerIndex < rhs.m_LayerIndex;
				});
		}

		Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
		auto layeredView = m_Registry.view<DefaultGroupComponent, QuadComponent>();
		layeredView.use<DefaultGroupComponent>();
		layeredView.each([&](auto entity, DefaultGroupComponent& sortGroup, QuadComponent& quad)
			{
				static uint32_t prevLayer = sortGroup.m_LayerIndex;
				
				if (prevLayer != sortGroup.m_LayerIndex)
				{
					Renderer2D::Instance()->EndScene();
					Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
				}

				if (m_Registry.all_of<SpriteComponent>(entity))
				{
					auto& sprite = m_Registry.get<SpriteComponent>(entity);

					if (sprite.m_Disabled == false)
					{
						if (sprite.m_Enable)
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite, m_DefaultShader.get());
						else
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, sprite.m_Color, m_DefaultSolidColorShader.get());
					}
				}
				else if (m_Registry.all_of<AnimatedSpriteComponent>(entity))
				{
					auto& anim = m_Registry.get<AnimatedSpriteComponent>(entity);
					if (anim.IsPlaying() || m_RuntimeState == RuntimeState::STOPPED)
						Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, anim, m_DefaultShader.get());
				}

				prevLayer = sortGroup.m_LayerIndex;
			});
		
		Renderer2D::Instance()->EndScene();

		// TODO: Move Circle as a part of Renderer component (Circle + Sprite doesn't make much sense)
		auto circleView = m_Registry.view<CircleComponent>();
		if (circleView)
		{
			circleView.each([this](CircleComponent& circle)
				{
					Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
				});
		}
	}

	void Scene::OnSceneStep()
	{
		ST_PROFILE_FUNCTION();

		Timestep ts = Time::Instance()->GetFrameTime();

		// Update Scripts
		UpdateScripts(ts);

		// Physics
		UpdatePhysicsSimulation(ts);

		// Update components that depend on timestep
		UpdateComponents(ts);

		UpdateComponentDependacies(ts);
	}

	void Scene::OnEvent(Event* event)
	{
		EventDispatcher<PhysicsEvent> eventDispatcher;
		eventDispatcher.Dispatch(event, [&](PhysicsEvent* e) -> bool {
			return ProcessPhysicsEvent(e);;
		});
	}

	Entity Scene::CreateEntity(const std::string& name, const uint64_t& uuid)
	{
		Entity entity = { m_Registry.create(), this };
		UUID entityUUID = uuid == 0 ? UUID() : UUID(uuid);
		entity.AddComponent<UUIDComponent>(entityUUID);
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		entity.AddComponent<DefaultGroupComponent>();

		m_Entities.push_back(entity);
		m_EntityUUIDMap[entityUUID] = entity;
		return entity;
	}

	Entity Scene::GetEntityByHandle(entt::entity handle)
	{
		return Entity(handle, this);
	}

	Entity Scene::FindentityByName(std::string_view name)
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

	bool Scene::RemoveEntity(Entity entity)
	{
		bool valid = IsEntityValid(entity);
		if (valid)
		{
			m_EntityUUIDMap.erase(entity.GetComponent<UUIDComponent>().m_UUID);
			m_Registry.destroy(entity.GetHandle());
			m_Entities.erase(std::remove(std::begin(m_Entities), std::end(m_Entities), entity));
		}

		return valid;
	}


	bool Scene::RemoveEntity(entt::entity handle)
	{
		Entity entity = Entity{ handle, this };
		return RemoveEntity(entity);
	}

	bool Scene::IsEntityValid(Entity entity)
	{
		return m_Registry.valid(entity.GetHandle());
	}

	Entity Scene::CopyEntity(const Entity entity)
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

		// Generate a new UUID for the copy Entity
		UUID newUUID;
		newEntity.GetComponent<UUIDComponent>().m_UUID = newUUID;

		m_EntityUUIDMap[newUUID] = newEntity;
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
		ST_PROFILE_FUNCTION();

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
		m_RenderCamera = m_SceneCamera;
		m_RuntimeState = RuntimeState::PAUSED;
	}

	void Scene::OnSceneResume()
	{
		m_Registry.view<CameraComponent>().each([=](auto entity, auto& ncs)
		{
			if (ncs.m_IsMain)
			{
				m_RenderCamera = ncs.m_Camera.get();
			}
		});

		m_RuntimeState = RuntimeState::RUNNING;
	}

	void Scene::OnSceneStop()
	{
		ST_PROFILE_FUNCTION();

		DeinitializeScritps();
		DeinitializePhysics();

		// Update Scene state last
		m_RuntimeState = RuntimeState::STOPPED;

		Physics::ClearActiveCollisions();
	}

	void Scene::UpdateMainCamera()
	{
		bool mainCameraFound = false;

		m_Registry.view<CameraComponent>().each([=, &mainCameraFound](auto entity, auto& ncs)
			{
				if (ncs.m_IsMain)
				{
					m_RenderCamera = ncs.m_Camera.get();
					mainCameraFound = true;
				}
			});

		if (!mainCameraFound)
		{
			m_RenderCamera = m_SceneCamera;
		}
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

	Entity Scene::MousePickEntity(float x, float y)
	{
		Entity picked = {};

		auto view = m_Registry.view<DefaultGroupComponent>();
		for (auto it = view.rbegin(), last = view.rend(); it != last; ++it)
		{
			Entity entity = Entity(*it, this);
			if (entity.HasComponent<QuadComponent>())
			{
				QuadComponent quad = entity.GetComponent<QuadComponent>();
				if (quad.m_PickEnabled && SceneUtils::IsPointInRotatedSquare({ x, y }, quad.Center(), quad.m_Size, quad.m_Rotation))
				{
					picked = entity;
					break;
				}
			}
			else if (entity.HasComponent<CircleComponent>())
			{
				CircleComponent circle = entity.GetComponent<CircleComponent>();
				if (SceneUtils::IsPointInCircle({ x, y }, circle.Center(), circle.MaxRadius() / 2.0f))
				{
					picked = entity;
					break;
				}
			}
		}

		/*GraphicsConfig graphicsConfig = Project::GetGraphicsConfig();
		auto& sortingLayers = Project::GetSortingLayers();
		auto& entityGroups = m_EntitySorter.GetEntityGroups();
		for (auto& riter = sortingLayers.rbegin(); riter != sortingLayers.rend(); riter++)
		{
			auto& layer = *riter;
			auto& entityLayerGroup = entityGroups[layer->m_Name];
			if (!entityLayerGroup.m_Entities.empty())
			{
				// Reverse iterate trough entities and select first "hit"
				for (auto& reiter = entityLayerGroup.m_Entities.rbegin(); reiter != entityLayerGroup.m_Entities.rend(); reiter++)
				{
					Entity entity = Entity((entt::entity)reiter->m_EntityID, this);
					if (entity.HasComponent<QuadComponent>())
					{
						QuadComponent quad = entity.GetComponent<QuadComponent>();
						if (quad.m_PickEnabled && SceneUtils::IsPointInRotatedSquare({ x, y }, quad.Center(), quad.m_Size, quad.m_Rotation))
						{
							picked = entity;
							break;
						}
					}
				}
			}

			// Stop if we already found a "hit" in layers
			if (picked)
				break;

			// Check Axis sorted Entities
			if (layer->m_Name == "Default")
			{
				for (auto& reiter = m_EntitySorter.m_AxisSortedEntites.rbegin(); reiter != m_EntitySorter.m_AxisSortedEntites.rend(); reiter++)
				{
					Entity entity = Entity((entt::entity)*reiter, this);
					if (entity.HasComponent<QuadComponent>())
					{
						QuadComponent quad = entity.GetComponent<QuadComponent>();
						if (quad.m_PickEnabled && SceneUtils::IsPointInRotatedSquare({ x, y }, quad.Center(), quad.m_Size, quad.m_Rotation))
						{
							picked = entity;
							break;
						}
					}
				}
			}

			// Stop if we already found a "hit" in Axis sorted Entities
			if (picked)
				break;
		}

		m_Registry.view<CircleComponent>().each([this, &picked, x, y](auto e, auto& circle)
			{
				if (SceneUtils::IsPointInCircle({ x, y }, circle.Center(), circle.MaxRadius() / 2.0f))
				{
					picked = Entity(e, this);
				}
			});

		// Collider box only entities will have priority to be selected, acting as always on top as they are rendered last.
		// This is only if the debug option is enabled and colliders are visible
		if (Physics::ShowColliderOutlineEnabled())
		{
			m_Registry.view<BoxCollider2DComponent, QuadComponent>().each([this, &picked, x, y](auto e, auto& collider, auto& quad)
			{
				if (SceneUtils::IsPointInRotatedSquare({ x, y }, quad.Center(), quad.m_Size, quad.m_Rotation))
				{
					picked = Entity(e, this);
				}
			});

			m_Registry.view<BoxCollider2DComponent, CircleComponent>().each([this, &picked, x, y](auto e, auto& collider, auto& circle)
			{
				if (SceneUtils::IsPointInCircle({ x, y }, circle.Center(), circle.MaxRadius() / 2.0f))
				{
					picked = Entity(e, this);
				}
			});
		}*/

		return picked;
	}

	void Scene::UpdateComponentDependacies(Timestep ts)
	{
		ST_PROFILE_FUNCTION();

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
		ST_PROFILE_FUNCTION();

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
		ST_PROFILE_FUNCTION();

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
		ST_PROFILE_FUNCTION();

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
		ST_PROFILE_FUNCTION();

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
		ST_PROFILE_FUNCTION();

		glm::vec2 gravityForce = Project::GetPhysicsConfig().m_GravityForce;

		m_PhysicsWorld = new b2World({ gravityForce.x, gravityForce.y });
		m_Registry.view<RigidBody2DComponent>().each([=](auto e, auto& rb2d)
			{
				Entity entity = { e, this };
				CreatePhysicsBody(entity);
			});

		m_PhysicsWorld->SetContactListener(m_ContactListener.get());
	}


	void Scene::CreatePhysicsBody(Entity entity)
	{
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			RigidBody2DComponent& rb2d = entity.GetComponent<RigidBody2DComponent>();
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

					fixtureDef.filter.groupIndex = bc2d.m_GroupIndex;

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
		}
	}

	void Scene::DestroyPhysicsBody(Entity entity)
	{
		if (entity.HasComponent<RigidBody2DComponent>())
		{
			RigidBody2DComponent& rb2d = entity.GetComponent<RigidBody2DComponent>();
			if (m_PhysicsWorld)
			{
				m_PhysicsWorld->DestroyBody((b2Body*)rb2d.m_RuntimeBody);
				rb2d.m_RuntimeBody = nullptr;
			}
		}
	}

	void Scene::SetPhysicsEntityState(Entity entity, bool enabled)
	{
		m_PhysicsStateToBeChanged[entity] = enabled;
	}


	bool Scene::IsPhysicsWorldLocked()
	{
		if (m_PhysicsWorld)
			return m_PhysicsWorld->IsLocked();

		return false;
	}

	void Scene::UpdatePhysicsSimulation(Timestep ts)
	{
		ST_PROFILE_FUNCTION();
		Clock::Begin();

		// Before stepping physics world, update Enable state
		UpdatePyhsicsEntityState();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		auto physicsView = m_Registry.group<RigidBody2DComponent, BoxCollider2DComponent>(entt::get<QuadComponent>);
		physicsView.each([&](RigidBody2DComponent& rb2d, BoxCollider2DComponent& bc2d, QuadComponent& quad)
			{
				// Complete deferred body updates
				if (rb2d.m_ShouldUpdateTransform)
				{
					rb2d.m_ShouldUpdateTransform = false;
					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					if (body)
					{
						b2Vec2 pos = b2Vec2(rb2d.m_DeferredTransformPos.x, rb2d.m_DeferredTransformPos.y);
						body->SetTransform(pos, rb2d.m_DeferredTransfomrAngle);
					}
				}

				b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
				if (body)
				{
					const auto& position = body->GetPosition();
					quad.m_Position.x = position.x;
					quad.m_Position.y = position.y;
					quad.m_Rotation = body->GetAngle();

					// Updated quad position by Collider offset
					quad.m_Position.x -= bc2d.m_Offset.x;
					quad.m_Position.y -= bc2d.m_Offset.y;
				}
			});

		/*m_Registry.view<RigidBody2DComponent>().each([=](auto e, RigidBody2DComponent& rb2d)
			{
				// Complete deferred body updates
				if (rb2d.m_ShouldUpdateTransform)
				{
					rb2d.m_ShouldUpdateTransform = false;
					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					if (body)
					{
						b2Vec2 pos = b2Vec2(rb2d.m_DeferredTransformPos.x, rb2d.m_DeferredTransformPos.y);
						body->SetTransform(pos, rb2d.m_DeferredTransfomrAngle);
					}
				}

				Entity entity = { e, this };
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();

					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					if (body)
					{
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
					else
					{
						auto& tag = entity.GetComponent<TagComponent>();
						ST_CORE_WARN("Entity {} has no Physics Body initialized!", tag.m_Tag);
					}
				}

				if (entity.HasComponent<CircleComponent>())
				{
					auto& circle = entity.GetComponent<CircleComponent>();

					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					if (body)
					{
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
					else
					{
						auto& tag = entity.GetComponent<TagComponent>();
						ST_CORE_WARN("Entity {} has no Physics Body initialized!", tag.m_Tag);
					}
				}
			});*/

		Statistics::SetPhysicsSimTime(Clock::Stop());
	}

	void Scene::DeinitializePhysics()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	bool Scene::ProcessPhysicsEvent(PhysicsEvent* event)
	{
		ST_CORE_WARN("ProcessPhysicsEvent not currently used! Collision is handled in place.");

		return false;
	}


	void Scene::UpdatePyhsicsEntityState()
	{
		ST_PROFILE_FUNCTION();

		for (auto& item : m_PhysicsStateToBeChanged)
		{
			Entity entity = { (entt::entity)item.first, this };
			bool enable = item.second;
			if (entity.HasComponent<RigidBody2DComponent>())
			{
				auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
				if (rb2d.m_RuntimeBody)
				{
					b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
					body->SetEnabled(enable);
				}
			}
		}

		m_PhysicsStateToBeChanged.clear();
	}

	void Scene::OnDebugUpdate(Timestep ts)
	{
		ST_PROFILE_FUNCTION();

		// Debug render - Collision contact points
		if (Physics::ShowCollisionsContactPointsEnabled())
		{
			if (m_RenderCamera)
			{
				Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
		
				auto& activeCollisions = Physics::GetActiveCollisions();
				for (auto& collision : activeCollisions)
				{
					for (Contact& contact : collision->m_Contacts)
					{
						Renderer2D::Instance()->SubmitCircle({ contact.m_Point.x, contact.m_Point.y, 0.0f }, { 1.0f, 1.0f }, {0.8f, 0.8f, 0.2f, 1.0f}, 1.0f, 0.005f);
					}
				}
				Renderer2D::Instance()->EndScene();
			}
		}
	}


	void Scene::RenderDebugData()
	{
		Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
		
		m_Registry.view<BoxCollider2DComponent>().each([=](auto e, BoxCollider2DComponent& bc2d)
			{
				Entity entity = { e, this };
				if (entity.HasComponent<QuadComponent>())
				{
					auto& quad = entity.GetComponent<QuadComponent>();
					auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
					glm::vec4 outlineColor(0.80f, 0.3f, 0.2f, 1.0f);
					glm::vec3 outlinePos(quad.Center() + bc2d.m_Offset, 0.0f);
					glm::vec2 outlineSize(bc2d.m_Size.x * quad.m_Size.x * 2.0f, bc2d.m_Size.y * quad.m_Size.y * 2.0f);

					if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::BOX)
					{
						Renderer2D::Instance()->SubmitSquare(outlinePos, outlineSize, quad.m_Rotation, outlineColor);
					}
					else if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::CIRLCE)
					{
						glm::vec2 circleOutlineSize(bc2d.m_Size.x * quad.m_Size.x * 2.0f, bc2d.m_Size.x * quad.m_Size.x * 2.0f);
						Renderer2D::Instance()->SubmitCircle(outlinePos, circleOutlineSize, outlineColor, 0.06f, 0.0f);
					}
				}
				else if (entity.HasComponent<CircleComponent>())
				{
					auto& circle = entity.GetComponent<CircleComponent>();
					auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
					glm::vec4 outlineColor(0.80f, 0.3f, 0.2f, 1.0f);
					glm::vec3 outlinePos(circle.Center() + bc2d.m_Offset, 0.0f);
					glm::vec2 outlineSize(bc2d.m_Size.x * circle.m_Size.x * 2.0f, bc2d.m_Size.y * circle.m_Size.y * 2.0f);

					if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::BOX)
					{
						Renderer2D::Instance()->SubmitSquare(outlinePos, outlineSize, circle.m_Rotation, outlineColor);
					}
					else if (bc2d.m_ColliderShape == BoxCollider2DComponent::Collider2DShape::CIRLCE)
					{
						glm::vec2 circleOutlineSize(bc2d.m_Size.x * circle.m_Size.x * 2.0f, bc2d.m_Size.x * circle.m_Size.x * 2.0f);
						Renderer2D::Instance()->SubmitCircle(outlinePos, circleOutlineSize, outlineColor, 0.06f, 0.0f);
					}
				}
			});

		Renderer2D::Instance()->EndScene();
	}

}