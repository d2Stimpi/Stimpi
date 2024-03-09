#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"

#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/SubTexture.h"

#include "Stimpi/Scene/Camera.h"
#include "Stimpi/Scene/CameraController.h"

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
		void OnEvent(Event* event);

		Entity CreateEntity(const std::string& name = "");
		Entity GetEntityByHandle(entt::entity handle);
		Entity FindentityByName(std::string_view name);
		void RemoveEntity(Entity entity);

		Entity CopyEntity(const Entity entity);

		void SetCamera(Camera* camera);
		Camera* GetCamera() { return m_SceneCamera; }
		Camera* GetRenderCamera() { return m_RenderCamera; } // Use this for picking so it can work in runtime

		RuntimeState GetRuntimeState() { return m_RuntimeState; }

		void OnScenePlay();
		void OnScenePause();
		void OnSceneResume();
		void OnSceneStop();

		// Mouse Picking
		Entity MousePickEntity(float x, float y);
	private:
		// Update components based on interactions
		void UpdateComponentDependacies(Timestep ts);
		void UpdateComponents(Timestep ts);

		// Scripting
		void InitializeScripts();
		void UpdateScripts(Timestep ts);
		void DeinitializeScritps();

		// Physics
		void InitializePhysics();
		void UpdatePhysicsSimulation(Timestep ts);
		void DeinitializePhysics();
		bool ProcessPhysicsEvent(PhysicsEvent* event);

	private:
		entt::registry m_Registry;
		std::vector<Entity> m_Entities;
		RuntimeState m_RuntimeState;

		Camera* m_SceneCamera = nullptr; // Created outside of scene (Editor)
		Camera* m_RenderCamera = nullptr; // Scene will use Camera Component in Runtime state
		// Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;
		std::shared_ptr<Stimpi::Shader> m_DefaultSolidColorShader;

		// Test
		std::shared_ptr<SubTexture> m_SubTexture;
		Texture* m_TestTexture;

		// Physics
		b2World* m_PhysicsWorld = nullptr;
		std::shared_ptr <ContactListener> m_ContactListener;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyWindow;
		friend class SceneViewWindow;
		friend class ScriptEngine;
	};
}