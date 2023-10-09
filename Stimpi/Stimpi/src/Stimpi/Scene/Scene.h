#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"

#include "Stimpi/Gui/SceneConfigWindow.h"

#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/OrthoCamera.h" // TODO: make Camera class to use in scene

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

		std::shared_ptr<OrthoCamera> m_SceneCamera;
		//Temp shader
		std::shared_ptr<Stimpi::Shader> m_DefaultShader;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneConfigWindow;
	};
}