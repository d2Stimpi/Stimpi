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
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFucntions;
	static std::unordered_map<MonoType*, std::function<void(Entity)>> s_EntityCreateComponentFucntions;
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
			s_EntityCreateComponentFucntions[monoType] = [](Entity entity) { entity.AddComponent<QuadComponent>(); };
			s_EntityRemoveComponentFucntions[monoType] = [](Entity entity) { entity.RemoveComponent<QuadComponent>(); };
		}
		else
		{
			ST_CORE_TRACE("{} not registerd - not found in C#", managedName);
		}

	}
	

	static bool Entity_HasComponent(uint32_t entityID, MonoReflectionType* compType)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		MonoType* managedType = mono_reflection_type_get_type(compType);
		ST_CORE_ASSERT(s_EntityHasComponentFucntions.find(managedType) == s_EntityHasComponentFucntions.end(), "Component not registerd");
		return s_EntityHasComponentFucntions.at(managedType)(entity);
	}

	/* QuadComponent */

	static bool QuadComponent_GetPosition(uint32_t entityID, glm::vec2* outPosition)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);

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
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);

		if (entity.HasComponent<QuadComponent>())
		{
			auto& quad = entity.GetComponent<QuadComponent>();
			quad.m_Position = *position;
			hasComponent = true;
		}

		return hasComponent;
	}

	/* Input */

	static bool Input_IsKeyPressed(uint32_t keycode)
	{
		return InputManager::Instance()->IsKeyPressed(keycode);
	}

	void ScriptGlue::RegisterFucntions()
	{
		// Components
		ST_ADD_INTERNAL_CALL(Entity_HasComponent);

		// QuadComponent
		ST_ADD_INTERNAL_CALL(QuadComponent_GetPosition);
		ST_ADD_INTERNAL_CALL(QuadComponent_SetPosition);

		// Input
		ST_ADD_INTERNAL_CALL(Input_IsKeyPressed);
	}

	void ScriptGlue::RegosterComponents()
	{
		RegisterComponent<TagComponent>();
		RegisterComponent<QuadComponent>();
		RegisterComponent<TextureComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<RigidBody2DComponent>();
		RegisterComponent<BoxCollider2DComponent>();
	}

}