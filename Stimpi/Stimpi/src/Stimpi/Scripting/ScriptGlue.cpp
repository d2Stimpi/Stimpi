#include "stpch.h"
#include "Stimpi/Scripting/ScriptGlue.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/KeyCodes.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

#include "box2d/b2_body.h"
#include "box2d/b2_math.h"

#include "mono/metadata/appdomain.h"
#include "mono/metadata/reflection.h"

#include <glm/glm.hpp>

#define ST_ADD_INTERNAL_CALL(Name)	mono_add_internal_call("Stimpi.InternalCalls::" #Name, Name);

namespace Stimpi
{
#pragma region Component

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFucntions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityAddComponentFucntions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityRemoveComponentFucntions;

	/* Components */
	template <typename TComponent>
	static void RegisterComponent()
	{
		std::string_view typeName = typeid(TComponent).name();
		size_t pos = typeName.find_last_of(':');
		std::string_view componentName = typeName.substr(pos + 1);
		std::string managedName = fmt::format("Stimpi.{}", componentName);

		MonoType* monoType = mono_reflection_type_from_name(managedName.data(), ScriptEngine::GetCoreAssemblyImage());
		if (monoType != nullptr)
		{
			s_EntityHasComponentFucntions[monoType] = [](Entity entity) { return entity.HasComponent<TComponent>(); };
			s_EntityAddComponentFucntions[monoType] = [](Entity entity) { entity.AddComponent<TComponent>(); };
			s_EntityRemoveComponentFucntions[monoType] = [](Entity entity) { entity.RemoveComponent<TComponent>(); };
		}
		else
		{
			ST_CORE_TRACE("{} not registerd - not found in C#", managedName);
		}

	}

	static bool Entity_HasComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ST_CORE_ASSERT_MSG(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		return s_EntityHasComponentFucntions.at(managedType)(entity);
	}

	static bool Entity_AddComponent(uint32_t entityID, MonoReflectionType* componentType)
	{

		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ST_CORE_ASSERT_MSG(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		s_EntityAddComponentFucntions.at(managedType)(entity);
		return s_EntityHasComponentFucntions.at(managedType)(entity);
	}

	static bool Entity_RemoveComponent(uint32_t entityID, MonoReflectionType* componentType)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ST_CORE_ASSERT_MSG(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		s_EntityRemoveComponentFucntions.at(managedType)(entity);
		return !s_EntityHasComponentFucntions.at(managedType)(entity);
	}

	static uint32_t Entity_FindEntityByName(MonoString* name)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		
		char* nameCStr = mono_string_to_utf8(name);
		Entity entity = scene->FindentityByName(nameCStr);
		mono_free(nameCStr);

		if (!entity)
			return 0;

		return entity;
	}

	static MonoObject* GetScriptInstace(uint32_t entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}
	

#pragma endregion Component

#pragma region TagComponent

	static void TagComponent_GetString(uint32_t entityID, MonoString** outTag)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);
		bool hasTag = entity.HasComponent<TagComponent>();
		ST_CORE_ASSERT(!hasTag);

		std::string tagName = entity.GetComponent<TagComponent>().m_Tag;
		*outTag = mono_string_new(ScriptEngine::GetAppDomain(), tagName.c_str());
	}

#pragma endregion TagComponent

#pragma region QuadComponent

	static bool QuadComponent_GetPosition(uint32_t entityID, glm::vec2* outPosition)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			*outPosition = entity.GetComponent<QuadComponent>().m_Position;
			hasComponent = true;
		}

		return hasComponent;
	}
	
	// TODO: handle Z axis
	static bool QuadComponent_SetPosition(uint32_t entityID, glm::vec2* position)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_Position.x = (*position).x;
			quad.m_Position.y = (*position).y;
			hasComponent = true;
		}

		return hasComponent;
	}

	static bool QuadComponent_GetSize(uint32_t entityID, glm::vec2* outSize)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			*outSize = entity.GetComponent<QuadComponent>().m_Size;
			hasComponent = true;
		}

		return hasComponent;
	}

	static bool QuadComponent_SetSize(uint32_t entityID, glm::vec2* size)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_Size = *size;
			hasComponent = true;
		}

		return hasComponent;
	}

	static bool QuadComponent_GetRotation(uint32_t entityID, float* outRotation)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			*outRotation = entity.GetComponent<QuadComponent>().m_Rotation;
			hasComponent = true;
		}

		return hasComponent;
	}

	static bool QuadComponent_SetRotation(uint32_t entityID, float* rotation)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_Rotation = *rotation;
			hasComponent = true;
		}

		return hasComponent;
	}

#pragma endregion QuadComponent

#pragma region SpriteComponent

	static bool SpriteComponent_GetColor(uint32_t entityID, glm::vec4* outColor)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<SpriteComponent>())
		{
			*outColor = entity.GetComponent<SpriteComponent>().m_Color;
			hasComponent = true;
		}

		return hasComponent;
	}

	static bool SpriteComponent_SetColor(uint32_t entityID, glm::vec4* color)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<SpriteComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();
			sprite.m_Color = *color;
			hasComponent = true;
		}

		return hasComponent;
	}

#pragma endregion SpriteComponent

#pragma region AnimatedSpriteComponent

	static bool AnimatedSpriteComponent_IsAnimationSet(uint32_t entityID, bool* isSet)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<AnimatedSpriteComponent>();
		if (hasComponent)
		{
			auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
			*isSet = anim.IsAnimationSet();
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_AnimStart(uint32_t entityID)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<AnimatedSpriteComponent>();
		if (hasComponent)
		{
			auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
			if (anim.IsAnimationSet())
			{
				anim.Start();
			}
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_AnimPause(uint32_t entityID)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<AnimatedSpriteComponent>();
		if (hasComponent)
		{
			auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
			if (anim.IsAnimationSet())
			{
				anim.Pause();
			}
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_AnimStop(uint32_t entityID)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<AnimatedSpriteComponent>();
		if (hasComponent)
		{
			auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
			if (anim.IsAnimationSet())
			{
				anim.Stop();
			}
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_GetAnimState(uint32_t entityID, int* outState)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<AnimatedSpriteComponent>();
		if (hasComponent)
		{
			auto& anim = entity.GetComponent<AnimatedSpriteComponent>();
			if (anim.IsAnimationSet())
			{
				*outState = (int)anim.m_AnimSprite->GetAnimationState();
			}
		}

		return hasComponent;
	}

#pragma endregion AnimatedSpriteComponent

#pragma region Input

	static bool Input_IsKeyDown(uint32_t keycode)
	{
		return InputManager::Instance()->IsKeyDown(keycode);
	}

	static bool Input_IsKeyPressed(uint32_t keycode)
	{
		return InputManager::Instance()->IsKeyPressed(keycode);
	}

	static bool Input_IsKeyUp(uint32_t keycode)
	{
		return InputManager::Instance()->IsKeyUp(keycode);
	}

	static bool Input_IsMouseDown(uint32_t mousecode)
	{
		return InputManager::Instance()->IsMouseButtonDown(mousecode);
	}

	static bool Input_IsMousePressed(uint32_t mousecode)
	{
		return InputManager::Instance()->IsMouseButtonPressed(mousecode);
	}

	static bool Input_IsMouseUp(uint32_t mousecode)
	{
		return InputManager::Instance()->IsMouseButtonUp(mousecode);
	}

#pragma endregion Input

#pragma region RigidBody2DComponent

	static bool RigidBody2DComponent_GetRigidBodyType(uint32_t entityID, int* outType)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			*outType = (int)entity.GetComponent<RigidBody2DComponent>().m_Type;
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_SetRigidBodyType(uint32_t entityID, int type)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			rb2d.m_Type = (RigidBody2DComponent::BodyType)type;
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_GetFixedRotation(uint32_t entityID, bool* outFixedRotation)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			*outFixedRotation = (int)entity.GetComponent<RigidBody2DComponent>().m_FixedRotation;
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_SetFixedRotation(uint32_t entityID, bool fixedRotation)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			rb2d.m_FixedRotation = fixedRotation;
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_GetTransform(uint32_t entityID, glm::vec2* outPosition, float* outAngle)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			b2Transform transform = body->GetTransform();
			
			*outPosition = glm::vec2(transform.p.x, transform.p.y);
			*outAngle = transform.q.GetAngle();
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_SetTransform(uint32_t entityID, glm::vec2* position, float angle)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			b2Vec2 pos = b2Vec2(position->x, position->y);
			body->SetTransform(pos, angle);
		}

		return hasComponent;
	}

#pragma endregion RigidBody2DComponent

#pragma region Pysics

	// TODO: move under RigidBody2D section
	static bool Physics_ApplyForce(uint32_t entityID, glm::vec2* force, glm::vec2* point, bool wake)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			body->ApplyForce({ force->x, force->y }, { point->x, point->y }, wake);
		}

		return hasComponent;
	}

	// TODO: move under RigidBody2D section
	static bool Physics_ApplyForceCenter(uint32_t entityID, glm::vec2* force, bool wake)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			body->ApplyForceToCenter({ force->x, force->y }, wake);
		}

		return hasComponent;
	}

	// TODO: move under RigidBody2D section
	static bool Physics_ApplyLinearImpulse(uint32_t entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			body->ApplyLinearImpulse({ impulse->x, impulse->y }, { point->x, point->y }, wake);
		}

		return hasComponent;
	}

	// TODO: move under RigidBody2D section
	static bool Physics_ApplyLinearImpulseCenter(uint32_t entityID, glm::vec2* impulse, bool wake)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			body->ApplyLinearImpulseToCenter({ impulse->x, impulse->y }, wake);
		}

		return hasComponent;
	}

	static bool Physics_GetLinearVelocity(uint32_t entityID, glm::vec2* velocity)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			auto value = body->GetLinearVelocity();
			*velocity = { value.x, value.y };
		}

		return hasComponent;
	}

	static bool Physics_SetLinearVelocity(uint32_t entityID, glm::vec2* velocity)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<RigidBody2DComponent>();
		if (hasComponent)
		{
			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
			body->SetLinearVelocity({velocity->x, velocity->y});
		}

		return hasComponent;
	}

#pragma endregion Pysics

#pragma region Collision

	static bool Collision_GetImpactVelocity(uint32_t entityID, uint32_t otherID, glm::vec2* velocity)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);

		Collision* collision = Physics::FindCollision(entityID, otherID);
		if (collision)
		{
			*velocity = collision->m_ImpactVelocity;
			return true;
		}

		return false;
	}

	static MonoArray* Collision_GetContacts(uint32_t entityID, uint32_t otherID)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);

		Collision* collision = Physics::FindCollision(entityID, otherID);
		if (collision)
		{
			MonoType* monoType = mono_reflection_type_from_name("Stimpi.Contact", ScriptEngine::GetCoreAssemblyImage());
			if (monoType)
			{
				uint32_t size = collision->m_Contacts.size();
				MonoClass* klass = mono_class_from_mono_type(monoType); // C# - ContactPoint struct
				MonoArray* monoArray = mono_array_new(ScriptEngine::GetAppDomain(), klass, size);
				MonoReflectionType* monoReflectionType = mono_type_get_object(ScriptEngine::GetAppDomain(), monoType);
				uint32_t i = 0;
				for (auto& contact : collision->m_Contacts)
				{
					MonoObject* object = mono_object_new(ScriptEngine::GetAppDomain(), mono_class_from_mono_type(monoType));
					mono_runtime_object_init(object);
					MonoClassField* point1Field = mono_class_get_field_from_name(klass, "Point");
					mono_field_set_value(object, point1Field, &contact.m_Point);
					MonoClassField* pointImpactVelocty = mono_class_get_field_from_name(klass, "ImpactVelocty");
					mono_field_set_value(object, pointImpactVelocty, &contact.m_ImpactVelocity);
					mono_array_set(monoArray, MonoObject*, i++, object);
				}
				return monoArray;
			}
		}

		return nullptr;
	}

#pragma endregion Collision

#pragma region Camera

	static bool CameraComponent_GetIsMain(uint32_t entityID, bool* isMain)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<CameraComponent>();
		if (hasComponent)
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			*isMain = camera.m_IsMain;
		}

		return hasComponent;
	}

	static bool CameraComponent_SetIsMain(uint32_t entityID, bool isMain)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<CameraComponent>();
		if (hasComponent)
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			camera.m_IsMain = isMain;
			// Trigger "Main" camera update
			scene->UpdateMainCamera();
		}

		return hasComponent;
	}

	static bool CameraComponent_GetZoom(uint32_t entityID, float* zoom)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<CameraComponent>();
		if (hasComponent)
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (camera.m_Camera)
				*zoom = camera.m_Camera->GetZoomFactor();
		}

		return hasComponent;
	}

	static bool CameraComponent_SetZoom(uint32_t entityID, float zoom)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<CameraComponent>();
		if (hasComponent)
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (camera.m_Camera)
				camera.m_Camera->SetZoomFactor(zoom);
		}

		return hasComponent;
	}

#pragma endregion Camera


	void ScriptGlue::RegisterFucntions()
	{
		// Components
		ST_ADD_INTERNAL_CALL(Entity_HasComponent);
		ST_ADD_INTERNAL_CALL(Entity_AddComponent);
		ST_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		ST_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		ST_ADD_INTERNAL_CALL(GetScriptInstace);

		// TagComponent
		ST_ADD_INTERNAL_CALL(TagComponent_GetString);

		// QuadComponent
		ST_ADD_INTERNAL_CALL(QuadComponent_GetPosition);
		ST_ADD_INTERNAL_CALL(QuadComponent_SetPosition);
		ST_ADD_INTERNAL_CALL(QuadComponent_GetSize);
		ST_ADD_INTERNAL_CALL(QuadComponent_SetSize);
		ST_ADD_INTERNAL_CALL(QuadComponent_GetRotation);
		ST_ADD_INTERNAL_CALL(QuadComponent_SetRotation);

		// SpriteComponent
		ST_ADD_INTERNAL_CALL(SpriteComponent_GetColor);
		ST_ADD_INTERNAL_CALL(SpriteComponent_SetColor);

		/// AnimatedSpriteComponent
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_IsAnimationSet);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimStart);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimPause);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimStop);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_GetAnimState);

		// RigidBody2DComponent
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetFixedRotation);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetFixedRotation);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetTransform);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetTransform);

		// CameraComponent
		ST_ADD_INTERNAL_CALL(CameraComponent_GetIsMain);
		ST_ADD_INTERNAL_CALL(CameraComponent_SetIsMain);
		ST_ADD_INTERNAL_CALL(CameraComponent_GetZoom);
		ST_ADD_INTERNAL_CALL(CameraComponent_SetZoom);

		// Input
		ST_ADD_INTERNAL_CALL(Input_IsKeyDown);
		ST_ADD_INTERNAL_CALL(Input_IsKeyPressed);
		ST_ADD_INTERNAL_CALL(Input_IsKeyUp);
		ST_ADD_INTERNAL_CALL(Input_IsMouseDown);
		ST_ADD_INTERNAL_CALL(Input_IsMousePressed);
		ST_ADD_INTERNAL_CALL(Input_IsMouseUp);

		// Physics
		ST_ADD_INTERNAL_CALL(Physics_ApplyForce);
		ST_ADD_INTERNAL_CALL(Physics_ApplyForceCenter);
		ST_ADD_INTERNAL_CALL(Physics_ApplyLinearImpulse);
		ST_ADD_INTERNAL_CALL(Physics_ApplyLinearImpulseCenter);
		ST_ADD_INTERNAL_CALL(Physics_GetLinearVelocity);
		ST_ADD_INTERNAL_CALL(Physics_SetLinearVelocity);

		// Collisions
		ST_ADD_INTERNAL_CALL(Collision_GetImpactVelocity);
		ST_ADD_INTERNAL_CALL(Collision_GetContacts);
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent<TagComponent>();
		RegisterComponent<QuadComponent>();
		RegisterComponent<CircleComponent>();
		RegisterComponent<SpriteComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<RigidBody2DComponent>();
		RegisterComponent<BoxCollider2DComponent>();
		RegisterComponent<AnimatedSpriteComponent>();
	}

#pragma region FieldDataTypeMapping

#define ST_ADD_FIELD_TYPE_READ(FieldName, ReadFunc)			s_FieldDataTypeReadFunctions[FieldName] = ReadFunc;
#define ST_ADD_FIELD_TYPE_WRITE(FieldName, WriteFunc)		s_FieldDataTypeWriteFunctions[FieldName] = WriteFunc;

	static std::unordered_map<std::string, std::function<void(MonoObject*, void*)>> s_FieldDataTypeReadFunctions;
	static std::unordered_map<std::string, std::function<void(MonoObject*, void*)>> s_FieldDataTypeWriteFunctions;

	static void Entity_Unpack(MonoObject* object, void* data)
	{
		MonoType* monoType = mono_reflection_type_from_name("Stimpi.Entity", ScriptEngine::GetCoreAssemblyImage());
		if (monoType)
		{
			MonoClass* klass = mono_class_from_mono_type(monoType);
			auto objClass = mono_object_get_class(object);
			auto cam = mono_class_get_field_from_name(objClass, "camera");

			MonoClassField* nested;
			void* iter = NULL;
			while ((nested = mono_class_get_fields(objClass, &iter))) {
				ST_CORE_INFO("** {}", mono_field_get_name(nested));

				MonoType* type = mono_field_get_type(nested);
				MonoClass* subClass = mono_type_get_class(type);

				if (subClass)
				{
					MonoClassField* nested2;
					void* iter2 = NULL;
					while ((nested2 = mono_class_get_fields(subClass, &iter2))) {
						ST_CORE_INFO("**** {}", mono_field_get_name(nested2));
					}
				}
			}

			MonoClassField* entity = mono_class_get_field_from_name(klass, "ID");
			mono_field_get_value(object, entity, data);
		}
	}

	static void Entity_Pack(MonoObject* object, void* data)
	{
		MonoType* monoType = mono_reflection_type_from_name("Stimpi.Entity", ScriptEngine::GetCoreAssemblyImage());
		if (monoType)
		{
			MonoClass* klass = mono_class_from_mono_type(monoType);
			MonoClassField* entity = mono_class_get_field_from_name(klass, "ID");
			mono_field_set_value(object, entity, data);
		}
	}

	void ScriptGlue::RegisterDataMappings()
	{
		ST_ADD_FIELD_TYPE_READ("Stimpi.Entity",		Entity_Unpack);
		ST_ADD_FIELD_TYPE_WRITE("Stimpi.Entity",	Entity_Pack);
	}


	void ScriptGlue::GetFieldValue(MonoObject* object, const std::string& name, void* data)
	{
		if (s_FieldDataTypeReadFunctions.find(name) != s_FieldDataTypeReadFunctions.end())
			s_FieldDataTypeReadFunctions.at(name)(object, data);
	}


	void ScriptGlue::SetFieldValue(MonoObject* object, const std::string& name, void* data)
	{
		if (s_FieldDataTypeWriteFunctions.find(name) != s_FieldDataTypeWriteFunctions.end())
			s_FieldDataTypeWriteFunctions.at(name)(object, data);
	}

#pragma endregion FieldDataTypeMapping

}