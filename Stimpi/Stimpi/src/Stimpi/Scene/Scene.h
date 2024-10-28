#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Core/UUID.h"

#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/SubTexture.h"

#include "Stimpi/Scene/Camera.h"
#include "Stimpi/Scene/CameraController.h"
#include "Stimpi/Scene/EntitySorter.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

class b2World;

namespace Stimpi
{
	class Entity;
	class ContactListener;

	enum class RuntimeState { STOPPED = 0, RUNNING, PAUSED };

	class ST_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);
		void OnSceneStep();
		void OnEvent(Event* event);

		Entity CreateEntity(const std::string& name = "", const uint64_t& uuid = 0);
		Entity GetEntityByHandle(entt::entity handle);
		Entity FindentityByName(std::string_view name);
		bool RemoveEntity(Entity entity);
		bool RemoveEntity(entt::entity handle);
		bool IsEntityValid(Entity entity);

		Entity CopyEntity(const Entity entity);

		void SetCamera(Camera* camera);
		Camera* GetCamera() { return m_SceneCamera; }
		Camera* GetRenderCamera() { return m_RenderCamera; } // Use this for picking so it can work in runtime

		RuntimeState GetRuntimeState() { return m_RuntimeState; }

		std::string GetName() { return m_Name; }
		void SetName(std::string name) { m_Name = name; }

		void OnScenePlay();
		void OnScenePause();
		void OnSceneResume();
		void OnSceneStop();

		// Camera
		void UpdateMainCamera();

		// Component change events
		void OnSortingLayerRemove(const std::string layerName);

		// Entity sorting
		EntitySorter& GetEntitySorter();
		void OnSortingAxisChange();
		void UpdateLayerSorting(Entity entity);

		// TODO: move physics related stuff to Physics class (Wrapper around Box2D)
		// Physics
		void CreatePhysicsBody(Entity entity);
		void DestroyPhysicsBody(Entity entity);
		void SetPhysicsEntityState(Entity entity, bool enabled);
		bool IsPhysicsWorldLocked();

		// Debugging
		static void EnableDebugMode(bool enable);

		// Mouse Picking
		Entity MousePickEntity(float x, float y);
	private:
		// Update components based on interactions
		void UpdateComponentDependacies(Timestep ts);
		void UpdateComponents(Timestep ts);

		// Rendering and sorting
		void SubmitForRendering(std::vector<Entity>& entities);

		// Scripting
		void InitializeScripts();
		void UpdateScripts(Timestep ts);
		void DeinitializeScritps();

		// Physics
		void InitializePhysics();
		void UpdatePhysicsSimulation(Timestep ts);
		void DeinitializePhysics();
		bool ProcessPhysicsEvent(PhysicsEvent* event);
		void UpdatePyhsicsEntityState();

		// Debug
		void OnDebugUpdate(Timestep ts);
		void RenderDebugData();

	private:
		entt::registry m_Registry;
		std::vector<Entity> m_Entities;
		std::unordered_map<UUID, Entity> m_EntityUUIDMap;
		RuntimeState m_RuntimeState;

		std::string m_Name = "Scene";

		Camera* m_SceneCamera = nullptr; // Created outside of scene (Editor)
		Camera* m_RenderCamera = nullptr; // Scene will use Camera Component in Runtime state
		// Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;
		std::shared_ptr<Stimpi::Shader> m_DefaultSolidColorShader;

		// Physics
		b2World* m_PhysicsWorld = nullptr;
		std::shared_ptr <ContactListener> m_ContactListener;
		std::unordered_map<uint32_t, bool> m_PhysicsStateToBeChanged;

		EntitySorter m_EntitySorter;

		friend class Entity;
		friend class EntityHierarchy;
		friend class SceneSerializer;
		friend class SceneHierarchyWindow;
		friend class SceneViewWindow;
		friend class ScriptEngine;
	};
}