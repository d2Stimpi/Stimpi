#include "Stimpi/Scene/Scene.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Graphics/Renderer2D.h"

namespace Stimpi
{
	// TODO: clean - don't create stuff here
	Scene::Scene()
	{
		/*auto entity = CreateEntity();
		entity.AddComponent<QuadComponent>(glm::vec4(0.0f, 0.0f, 50.0f, 40.0f));

		auto view = m_Registry.view<TagComponent>();
		for (auto item : view)
		{
			auto tag = view.get<TagComponent>(item);
		}

		auto textureObj = CreateEntity("Texture1");
		//textureObj.AddComponent<TextureComponent>("Picture1.jpg");
		textureObj.AddComponent<TextureComponent>("Capture.jpg");
		textureObj.AddComponent<QuadComponent>(glm::vec4(100.0f, 20.0f, 50.0f, 40.0f));

		textureObj = CreateEntity("Texture2");
		textureObj.AddComponent<TextureComponent>("Capture.jpg");
		textureObj.AddComponent<QuadComponent>(glm::vec4(200.0f, 20.0f, 50.0f, 40.0f));

		textureObj = CreateEntity("Texture3");
		textureObj.AddComponent<TextureComponent>("Picture1.jpg");
		textureObj.AddComponent<QuadComponent>(glm::vec4(300.0f, 20.0f, 50.0f, 40.0f));*/
	}

	// TODO: fix?
	Scene::Scene(std::string fileName)
	{
		//LoadSnece(fileName);
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep ts)
	{
		// Render all Square components
		/*auto view = m_Registry.view<QuadComponent>();
		for (auto entty : view)
		{
			auto quad = view.get<QuadComponent>(entty);
			Renderer2D::Instace()->Submit(quad);
		}*/

		for (auto entity : m_Entities)
		{
			if (entity.HasComponent<QuadComponent>())
			{
				auto quad = entity.GetComponent<QuadComponent>();
				if (entity.HasComponent<TextureComponent>())
				{
					auto texture = entity.GetComponent<TextureComponent>();
					Renderer2D::Instace()->Submit(quad, texture);
				}
				else
				{
					Renderer2D::Instace()->Submit(quad);
				}
			}
		}
	}

	void Scene::OnEvent(Event* event)
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);

		m_Entities.push_back(entity);
		return entity;
	}

	void Scene::Serialize(std::string fileName, std::string name)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		out << YAML::Key << name << YAML::Value;

			out << YAML::BeginMap;
			for (auto entity : m_Entities)
			{
				out << YAML::Key << "Entity" << YAML::Value;
				entity.Serialize(out);
			}
			out << YAML::EndMap;

		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(fileName, out.c_str());
	}

	void Scene::SaveScene(std::string fileName, std::string name)
	{
		Serialize(fileName, name);
	}

	void Scene::LoadSnece(std::string fileName, std::string name)
	{
		YAML::Node loadData = YAML::LoadFile(fileName);
		YAML::Node scene = loadData[name];

		YAML::Emitter emitter;
		emitter << scene;
		//ST_CORE_TRACE("Loading scene: {0}", name);
		//ST_CORE_TRACE("Entities:\n {0}", emitter.c_str());

		for (YAML::const_iterator it = scene.begin(); it != scene.end(); it++)
		{
			YAML::Node entityNode = it->second;
			Entity entity;
			if (entityNode["TagComponent"])
			{
				entity = CreateEntity(entityNode["TagComponent"]["Tag"].as<std::string>());
				if (entityNode["QuadComponent"])
				{
					entity.AddComponent<QuadComponent>(QuadComponent(entityNode["QuadComponent"]));
				}
				if (entityNode["TextureComponent"])
				{
					entity.AddComponent<TextureComponent>(TextureComponent(entityNode["TextureComponent"]));
				}
			}
		}
	}
}