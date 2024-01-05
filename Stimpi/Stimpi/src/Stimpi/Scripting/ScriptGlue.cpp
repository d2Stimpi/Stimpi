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
			quad.m_Position = *position;
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

#pragma region Input
	/* Input */

	static bool Input_IsKeyPressed(uint32_t keycode)
	{
		return InputManager::Instance()->IsKeyPressed(keycode);
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

#pragma endregion Pysics

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

		// RigidBody2DComponent
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetFixedRotation);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetFixedRotation);

		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetTransform);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetTransform);


		// Input
		ST_ADD_INTERNAL_CALL(Input_IsKeyPressed);

		// Physics
		ST_ADD_INTERNAL_CALL(Physics_ApplyForce);
		ST_ADD_INTERNAL_CALL(Physics_ApplyForceCenter);
		ST_ADD_INTERNAL_CALL(Physics_ApplyLinearImpulse);
		ST_ADD_INTERNAL_CALL(Physics_ApplyLinearImpulseCenter);
	}

	void ScriptGlue::RegosterComponents()
	{
		RegisterComponent<TagComponent>();
		RegisterComponent<QuadComponent>();
		RegisterComponent<SpriteComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<RigidBody2DComponent>();
		RegisterComponent<BoxCollider2DComponent>();
	}

}