#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"

#include "Stimpi/Graphics/Shader.h"

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

	private:
		entt::registry m_Registry;

		std::vector<Entity> m_Entities;

		std::shared_ptr<Camera> m_SceneCamera;
		std::shared_ptr<CameraController> m_CameraController;
		//Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyWindow;
	};
}