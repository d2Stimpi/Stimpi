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
		ST_PROFILE_FUNCTION();

		// Workaround - Create 0 value Entity and never use it. Fixes check for valid Entity
		m_Registry.create();
		ComponentObserver::InitComponentObservers(m_Registry, this);

		// Clear script instances - they are created in OnScriptConstruct
		ScriptEngine::ClearScriptInstances();

		m_RuntimeState = RuntimeState::STOPPED;
		m_DefaultShader.reset(Shader::CreateShader("shader.shader"));
		//m_DefaultShader.reset(Shader::CreateShader("pixelart.shader"));
		m_DefaultSolidColorShader.reset(Shader::CreateShader("solidcolor.shader"));

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

		/* When not in Running state, use Editor sourced camera */
		/*if (m_RuntimeState != RuntimeState::RUNNING)
		{
			m_RenderCamera = m_SceneCamera;
		}*/

		// Scene Rendering
		if (m_RenderCamera)
		{
			ST_PROFILE_SCOPE("SubmitForRendering");

			auto& sortingLayers = Project::GetSortingLayers();
			auto& entityGroups = m_EntitySorter.GetEntityGroups();
			for (auto& layer : sortingLayers)
			{
				ST_PROFILE_SCOPE("EntitySorter - Layer");

				auto& entityLayerGroup = entityGroups[layer->m_Name];

				// Pass filtered and sorted entities for rendering
				if (!entityLayerGroup.m_Entities.empty())
				{
					std::vector<Entity> renderEntities;
					for (auto item : entityLayerGroup.m_Entities)
						renderEntities.emplace_back((entt::entity)item.m_EntityID, this);

					Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());
					SubmitForRendering(renderEntities);
					Renderer2D::Instance()->EndScene();
				}

				// Render entities that don't use SortingGroup component as if in Default layer
				// TODO: make sure Default layer can't be removed or renamed
				if (layer->m_Name == Project::GetDefaultSortingLayerName())
				{
					Renderer2D::Instance()->BeginScene(m_RenderCamera->GetOrthoCamera());

					OnSortingAxisChange();

					std::vector<Entity> axisOrdered;
					for (auto entityID : m_EntitySorter.m_AxisSortedEntites)
						axisOrdered.emplace_back((entt::entity)entityID, this);

					SubmitForRendering(axisOrdered);

					Renderer2D::Instance()->EndScene();
				}
			}
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
		for (auto entity : entities)
		{
			if (entity.HasComponent<QuadComponent>())
			{
				auto& quad = entity.GetComponent<QuadComponent>();
				if (entity.HasComponent<SpriteComponent>())
				{
					auto& sprite = entity.GetComponent<SpriteComponent>();
					if (sprite.m_Disabled == false)
					{
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
				}

				// Draw AnimatedSprite if available (can overlap with Sprite)
				if (entity.HasComponent<AnimatedSpriteComponent>())
				{
					auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
					if (anim.Loaded())
					{
						// AnimationSprite is render always when in stopped "Editor" mode
						if (anim.IsPlaying() || m_RuntimeState == RuntimeState::STOPPED)
							Renderer2D::Instance()->Submit(quad.m_Position, quad.m_Size, quad.m_Rotation, anim, m_DefaultShader.get());
					}
				}

				
			}
			else if (entity.HasComponent<CircleComponent>())
			{
				auto& circle = entity.GetComponent<CircleComponent>();
				if (entity.HasComponent<SpriteComponent>())
				{
					auto& sprite = entity.GetComponent<SpriteComponent>();
					if (sprite.m_Disabled == false)
					{
						if (sprite.TextureLoaded() && sprite.m_Enable)
						{
							Renderer2D::Instance()->Submit(circle.m_Position, circle.m_Size, circle.m_Rotation, sprite, m_DefaultShader.get());
						}
						else
						{
							Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
						}
					}
				}
				else
				{
					Renderer2D::Instance()->SubmitCircle(circle.m_Position, circle.m_Size, circle.m_Color, circle.m_Thickness, circle.m_Fade);
				}

				
			}
		}
	}

	EntitySorter& Scene::GetEntitySorter()
	{
		return m_EntitySorter;
	}


	void Scene::OnSortingAxisChange()
	{
		m_EntitySorter.ResetAxisSortedEntites();

		GraphicsConfig graphicsConfig = Project::GetGraphicsConfig();
		if (graphicsConfig.m_RenderingOrderAxis != RenderingOrderAxis::None)
		{
			for (auto entity : m_Entities)
			{
				if (!entity.HasComponent<SortingGroupComponent>() && entity.HasComponent<QuadComponent>() &&
					(entity.HasComponent<SpriteComponent>() || entity.HasComponent<AnimatedSpriteComponent>()))
				{
					m_EntitySorter.SortQuadEntityByAxis(entity, graphicsConfig.m_RenderingOrderAxis);
				}
				else if (entity.HasComponent<CircleComponent>())
				{
					m_EntitySorter.SortCircleEntityByAxis(entity, graphicsConfig.m_RenderingOrderAxis);
				}
			}
		}
	}


	void Scene::UpdateLayerSorting(Entity entity)
	{
		if (!entity.HasComponent<SortingGroupComponent>())
			return;

		SortingGroupComponent group = entity.GetComponent<SortingGroupComponent>();

		m_EntitySorter.RemoveLayerSortedEntity(entity, group.m_SortingLayerName);
		m_EntitySorter.SortEntityByLayer({ entity, group.m_OrderInLayer }, group.m_SortingLayerName);
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
		entity.AddComponent<UUIDComponent>(uuid == 0 ? UUID() : UUID(uuid));
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

		m_Entities.push_back(entity);
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

		GraphicsConfig graphicsConfig = Project::GetGraphicsConfig();
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
		}

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

		// Before stepping physics world, update Enable state
		UpdatePyhsicsEntityState();

		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_PhysicsWorld->Step(ts, velocityIterations, positionIterations);

		m_Registry.view<RigidBody2DComponent>().each([=](auto e, RigidBody2DComponent& rb2d)
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
			});
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