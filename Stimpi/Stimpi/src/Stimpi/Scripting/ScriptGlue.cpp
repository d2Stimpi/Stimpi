#include "stpch.h"
#include "Stimpi/Scripting/ScriptGlue.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/KeyCodes.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

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
			s_EntityHasComponentFucntions[monoType] = [](Entity entity) { return entity.HasComponent<QuadComponent>(); };
			s_EntityAddComponentFucntions[monoType] = [](Entity entity) { entity.AddComponent<QuadComponent>(); };
			s_EntityRemoveComponentFucntions[monoType] = [](Entity entity) { entity.RemoveComponent<QuadComponent>(); };
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
		ST_CORE_ASSERT(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		return s_EntityHasComponentFucntions.at(managedType)(entity);
	}

	static bool Entity_AddComponent(uint32_t entityID, MonoReflectionType* componentType)
	{

		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		ST_CORE_ASSERT(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
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
		ST_CORE_ASSERT(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		s_EntityRemoveComponentFucntions.at(managedType)(entity);
		return !s_EntityHasComponentFucntions.at(managedType)(entity);
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

	void ScriptGlue::RegisterFucntions()
	{
		// Components
		ST_ADD_INTERNAL_CALL(Entity_HasComponent);
		ST_ADD_INTERNAL_CALL(Entity_AddComponent);
		ST_ADD_INTERNAL_CALL(Entity_RemoveComponent);

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

		// Input
		ST_ADD_INTERNAL_CALL(Input_IsKeyPressed);
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