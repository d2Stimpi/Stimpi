#include "stpch.h"
#include "Stimpi/Scene/SceneSerializer.h"

#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Entity.h"

namespace Stimpi
{

	SceneSerializer::SceneSerializer(Scene* scene)
		: m_Scene(scene)
	{
	}

	void SceneSerializer::Serialize(const std::string& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value;

		out << YAML::BeginMap;
		for (auto entity : m_Scene->m_Entities)
		{
			out << YAML::Key << "Entity" << YAML::Value;
			entity.Serialize(out);
		}
		out << YAML::EndMap;

		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath, out.c_str());
	}

	bool SceneSerializer::Deseriealize(const std::string& filePath)
	{
		YAML::Node loadData = YAML::LoadFile(filePath);
		YAML::Node scene = loadData["Scene"];

		YAML::Emitter emitter;
		emitter << scene;

		for (YAML::const_iterator it = scene.begin(); it != scene.end(); it++)
		{
			YAML::Node entityNode = it->second;
			Entity entity;	// TODO: move Entity deserialization code to Entity class
			if (entityNode["TagComponent"])
			{
				entity = m_Scene->CreateEntity(entityNode["TagComponent"]["Tag"].as<std::string>());
				if (entityNode["QuadComponent"])
				{
					entity.AddComponent<QuadComponent>(QuadComponent(entityNode["QuadComponent"]));
				}
				if (entityNode["TextureComponent"])
				{
					entity.AddComponent<TextureComponent>(TextureComponent(entityNode["TextureComponent"]));
				}
				if (entityNode["CameraComponent"])
				{
					entity.AddComponent<CameraComponent>(CameraComponent(entityNode["CameraComponent"]));
				}
			}
		}

		return true;	//TODO: error path?
	}

}