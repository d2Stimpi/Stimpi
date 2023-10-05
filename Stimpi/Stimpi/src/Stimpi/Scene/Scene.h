#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Timestep.h"

#include "Stimpi/Gui/SceneConfigWindow.h"

#include <entt/entt.hpp>
#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class Entity;

	class ST_API Scene
	{
	public:
		Scene();
		Scene(std::string fileName);
		~Scene();

		void OnUpdate(Timestep ts);
		void OnEvent(Event* event);

		Entity CreateEntity(const std::string& name = "");

		void Serialize(std::string fileName, std::string name);

		void SaveScene(std::string fileName, std::string name = "Scene");
		void LoadSnece(std::string fileName, std::string name = "Scene");

	private:
		entt::registry m_Registry;

		std::vector<Entity> m_Entities;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneConfigWindow;
	};
}