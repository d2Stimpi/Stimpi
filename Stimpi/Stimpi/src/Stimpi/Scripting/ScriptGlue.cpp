#include "stpch.h"
#include "Stimpi/Scripting/ScriptGlue.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/WindowManager.h"
#include "Stimpi/Core/KeyCodes.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/EntityManager.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

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

	static bool Entity_Remove(uint32_t entityID)
	{
		bool removed = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		if (scene->IsEntityValid(entity))
		{
			ScriptEngine::RemoveEntityScriptInstance(entityID);
		}

		return removed;
	}

	static bool Entity_IsValidEntityID(uint32_t entityID)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		return scene->IsEntityValid(entity);
	}

	static MonoObject* GetScriptInstace(uint32_t entityID)
	{
		return ScriptEngine::GetManagedInstance(entityID);
	}

	static MonoObject* CreateScriptInstance(MonoString* name)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);

		char* nameCStr = mono_string_to_utf8(name);
		Entity entity = scene->CreateEntity(nameCStr);
		entity.AddComponent<ScriptComponent>(nameCStr);
		mono_free(nameCStr);

		auto classInstance = ScriptEngine::GetScriptInstance(entity);
		if (classInstance)
			return classInstance->GetMonoInstance();

		return nullptr;
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

	static bool SpriteComponent_SetDisable(uint32_t entityID, bool disable)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		if (entity.HasComponent<SpriteComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();
			sprite.m_Disabled = disable;
			hasComponent = true;
		}

		return hasComponent;
	}

#pragma endregion SpriteComponent

#pragma region SortingGroupComponent

	static bool SortingGroupComponent_GetSortingLayerName(uint32_t entityID, MonoString** outLayerName)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<SortingGroupComponent>();
		if (hasComponent)
		{
			auto& sortingGroup = entity.GetComponent<SortingGroupComponent>();
			*outLayerName = mono_string_new(ScriptEngine::GetAppDomain(), sortingGroup.m_SortingLayerName.c_str());
		}

		return hasComponent;
	}

	static bool SortingGroupComponent_SetSortingLayerName(uint32_t entityID, MonoString* layerName)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<SortingGroupComponent>();
		if (hasComponent)
		{
			auto& sortingGroup = entity.GetComponent<SortingGroupComponent>();
			char* nameCStr = mono_string_to_utf8(layerName);
			sortingGroup.m_SortingLayerName = nameCStr;
			mono_free(nameCStr);
			sortingGroup.UpdateLayerIndex();

			EntityManager::UpdateEntitySortingLayerIndex(entity);
			
			// Trigger sorting update
			//scene->UpdateLayerSorting({ (entt::entity)entityID, scene });
		}

		return hasComponent;
	}

#pragma endregion SortingGroupComponent

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

	static bool AnimatedSpriteComponent_Play(uint32_t entityID, MonoString* animationName)
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
			char* nameCStr = mono_string_to_utf8(animationName);
			anim.SetAnimation(nameCStr);
			anim.Start();
			mono_free(nameCStr);
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_Stop(uint32_t entityID)
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
			anim.Stop();
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_Pause(uint32_t entityID)
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
			anim.Pause();
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_AddAnimation(uint32_t entityID, MonoString* assetName)
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
			char* nameCStr = mono_string_to_utf8(assetName);

			auto assetsPath = Project::GetAssestsDir();
			auto asset = assetsPath / nameCStr;

			anim.AddAnimation(asset.string());
			mono_free(nameCStr);
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_GetLooping(uint32_t entityID, bool* looping)
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
			*looping = anim.GetLooping();
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_SetLooping(uint32_t entityID, bool looping)
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
			anim.SetLooping(looping);
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_IsPlaying(uint32_t entityID, MonoString* animationName, bool* isPlaying)
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
			char* nameCStr = mono_string_to_utf8(animationName);

			*isPlaying = false;
			std::string activeAnimName = anim.GetActiveAnimationName();
			if (activeAnimName == nameCStr)
			{
				*isPlaying = anim.IsPlaying();
			}
			mono_free(nameCStr);
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_GetWrapMode(uint32_t entityID, int* outWrapeMode)
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
				*outWrapeMode = (int)anim.m_AnimSprite->GetWrapMode();
		}

		return hasComponent;
	}

	static bool AnimatedSpriteComponent_SetWrapMode(uint32_t entityID, int wrapeMode)
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
				anim.m_AnimSprite->SetWrapMode((AnimationWrapMode)wrapeMode);
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

	static bool Input_GetMousePosition(glm::vec2* outPosition)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto camera = scene->GetRenderCamera();
		ST_CORE_ASSERT(!camera);

		glm::vec2 mouseWinPos = InputManager::Instance()->GetMousePosition();
		glm::vec2 winSize = WindowManager::Instance()->GetWindowSize();
		glm::vec2 scenePos = SceneUtils::WindowToScenePosition(winSize, mouseWinPos);
		*outPosition = SceneUtils::WindowToWorldPoint(camera, winSize, scenePos);

		return true;
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
			if (scene->IsPhysicsWorldLocked())
			{
				rb2d.SetTransformDeferred(*position, angle);
			}
			else
			{
				b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
				if (body)
				{
					b2Vec2 pos = b2Vec2(position->x, position->y);
					body->SetTransform(pos, angle);
				}
			}
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_InitializePhysics2DBody(uint32_t entityID)
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
			if (rb2d.m_RuntimeBody == nullptr)
			{
				scene->CreatePhysicsBody(entity);
			}
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_SetDisabled(uint32_t entityID, bool enabled)
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
			if (rb2d.m_RuntimeBody)
			{
				scene->SetPhysicsEntityState(entity, enabled);
			}
		}

		return hasComponent;
	}

	static bool RigidBody2DComponent_IsDisabled(uint32_t entityID, bool* outEnabled)
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
			if (rb2d.m_RuntimeBody)
			{
				b2Body* body = (b2Body*)rb2d.m_RuntimeBody;
				*outEnabled = body->IsEnabled();
			}
		}

		return hasComponent;
	}

#pragma endregion RigidBody2DComponent

#pragma region BoxCollider2DComponent

	static bool BoxCollider2DComponent_GetShape(uint32_t entityID, int* outShape)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outShape = (int)bc2d.m_ColliderShape;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetShape(uint32_t entityID, int shape)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_ColliderShape = (BoxCollider2DComponent::Collider2DShape)shape;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetOffset(uint32_t entityID, glm::vec2* outOffset)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outOffset = bc2d.m_Offset;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetOffset(uint32_t entityID, glm::vec2* offset)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_Offset = *offset;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetSize(uint32_t entityID, glm::vec2* outSize)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outSize = bc2d.m_Size;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetSize(uint32_t entityID, glm::vec2* size)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_Size = *size;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetDensity(uint32_t entityID, float* outDensity)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outDensity = bc2d.m_Density;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetDensity(uint32_t entityID, float density)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_Density = density;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetFriction(uint32_t entityID, float* outFriction)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outFriction = bc2d.m_Friction;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetFriction(uint32_t entityID, float friction)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_Friction = friction;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetRestitution(uint32_t entityID, float* outRestitution)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outRestitution = bc2d.m_Restitution;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetRestitution(uint32_t entityID, float restitution)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_Restitution = restitution;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetRestitutionThreshold(uint32_t entityID, float* outRestitutionThreshold)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outRestitutionThreshold = bc2d.m_RestitutionThreshold;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetRestitutionThreshold(uint32_t entityID, float restitutionThreshold)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_RestitutionThreshold = restitutionThreshold;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_GetGroupIndex(uint32_t entityID, int16_t* outGroupIndex)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			*outGroupIndex = bc2d.m_GroupIndex;
		}

		return hasComponent;
	}

	static bool BoxCollider2DComponent_SetGroupIndex(uint32_t entityID, int16_t groupIndex)
	{
		bool hasComponent = false;
		auto scene = SceneManager::Instance()->GetActiveScene();
		ST_CORE_ASSERT(!scene);
		auto entity = scene->GetEntityByHandle((entt::entity)entityID);
		ST_CORE_ASSERT(!entity);

		hasComponent = entity.HasComponent<BoxCollider2DComponent>();
		if (hasComponent)
		{
			auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
			bc2d.m_GroupIndex = groupIndex;
		}

		return hasComponent;
	}

#pragma endregion BoxCollider2DComponent

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

		//Collision* collision = Physics::FindCollision(entityID, otherID);
		Collision* collision = Physics::GetActiveCollision();
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
		else
		{
			ST_CORE_WARN("Collision_GetContacts: Collision not found. IDs {} and {}", entityID, otherID);
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
		ST_ADD_INTERNAL_CALL(Entity_Remove);
		ST_ADD_INTERNAL_CALL(Entity_IsValidEntityID);
		ST_ADD_INTERNAL_CALL(GetScriptInstace);
		ST_ADD_INTERNAL_CALL(CreateScriptInstance);

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
		ST_ADD_INTERNAL_CALL(SpriteComponent_SetDisable);

		ST_ADD_INTERNAL_CALL(SortingGroupComponent_GetSortingLayerName);
		ST_ADD_INTERNAL_CALL(SortingGroupComponent_SetSortingLayerName);

		/// AnimatedSpriteComponent
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_IsAnimationSet);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimStart);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimPause);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AnimStop);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_GetAnimState);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_Play);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_Pause);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_Stop);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_AddAnimation);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_GetLooping);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_SetLooping);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_IsPlaying);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_GetWrapMode);
		ST_ADD_INTERNAL_CALL(AnimatedSpriteComponent_SetWrapMode);

		// RigidBody2DComponent
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetRigidBodyType);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetFixedRotation);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetFixedRotation);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_GetTransform);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetTransform);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_InitializePhysics2DBody);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_SetDisabled);
		ST_ADD_INTERNAL_CALL(RigidBody2DComponent_IsDisabled);

		// BoxCollider2DComponent
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetShape);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetShape);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetOffset);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetOffset);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetSize);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetSize);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetDensity);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetDensity);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetFriction);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetFriction);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetRestitution);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetRestitution);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetRestitutionThreshold);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetRestitutionThreshold);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetGroupIndex);
		ST_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetGroupIndex);

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
		ST_ADD_INTERNAL_CALL(Input_GetMousePosition);

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
		RegisterComponent<SortingGroupComponent>();
		RegisterComponent<CameraComponent>();
		RegisterComponent<RigidBody2DComponent>();
		RegisterComponent<BoxCollider2DComponent>();
		RegisterComponent<AnimatedSpriteComponent>();
	}
}