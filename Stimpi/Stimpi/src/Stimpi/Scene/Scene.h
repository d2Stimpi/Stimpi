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

namespace Stimpi
{
	class Entity;

	enum class RuntimeState { STOPPED = 0, RUNNING, PAUSED };

	class ST_API Scene
	{
	public:
		Scene();
		~Scene();

		void OnUpdate(Timestep ts);
		void OnEvent(Event* event);

		Entity CreateEntity(const std::string& name = "");

		void SetCamera(Camera* camera);
		Camera* GetCamera() { return m_SceneCamera; }

		RuntimeState GetRuntimeState() { return m_RuntimeState; }

		void OnScenePlay();
		void OnScenePause();
		void OnSceneStop();

	private:
		entt::registry m_Registry;
		std::vector<Entity> m_Entities;
		RuntimeState m_RuntimeState;

		Camera* m_SceneCamera = nullptr; // Created outside of scene (Editor)
		Camera* m_RenderCamera = nullptr; // Scene will use Camera Component in Runtime state
		//Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;

		//Test
		std::shared_ptr<SubTexture> m_SubTexture;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyWindow;
	};
}