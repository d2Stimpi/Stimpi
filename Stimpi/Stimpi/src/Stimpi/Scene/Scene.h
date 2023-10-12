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

	private:
		entt::registry m_Registry;

		std::vector<Entity> m_Entities;

		Camera* m_SceneCamera; // Created outside of scene (Editor); Scene will use Camera Component in Runtime state
		//Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;

		//Test
		std::shared_ptr<SubTexture> m_SubTexture;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyWindow;
	};
}