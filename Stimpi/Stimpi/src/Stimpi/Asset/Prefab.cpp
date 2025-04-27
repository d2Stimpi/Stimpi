#include "stpch.h"
#include "Stimpi/Asset/Prefab.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/Component.h"

#define SERIALIZE_ENTITY_COMPONENT(Entity, ComponentName, Emitter)			\
		if (Entity.HasComponent<ComponentName>())					\
		{															\
			Entity.GetComponent<ComponentName>().Serialize(Emitter);	\
		}

#define REMOVE_ENTITY_COMPONENT(Entity, ComponentName)	\
		if (Entity.HasComponent<ComponentName>())		\
		{												\
			Entity.RemoveComponent<ComponentName>();	\
		}

#define BUILD_ENTITY_COMPONENT(Entity, ComponentName, Node)	\
		if (Node[#ComponentName])						\
		{												\
			Entity.AddComponent<ComponentName>(ComponentName(Node[#ComponentName]));	\
		}

namespace Stimpi
{

	Prefab::Prefab(const std::string& name, YAML::Node data)
		: m_Name(name), m_Data(data)
	{
		ParseAssetData();
	}

	void Prefab::Initialize(Entity entity)
	{
		YAML::Emitter out;
		SerializeEntityList(entity, out);
		m_Data = YAML::Load(out.c_str());

		// Parse serialized data to internal maps
		ParseAssetData();
	}

	void Prefab::Save(const FilePath& filePath)
	{
		ResourceManager::Instance()->WriteToFile(filePath, m_Data);
	}

	Entity Prefab::CreateEntities(Scene* scene)
	{
		Entity root = scene->CreateEntity(m_Name);
		auto node = m_EntityDataMap[m_RootEntityUUID];
		BuildComponents(root, node);

		if (root.HasComponent<HierarchyComponent>())
		{
			HierarchyComponent& hierarchy = root.GetComponent<HierarchyComponent>();
			hierarchy.m_Children = m_HierarchyMap[m_RootEntityUUID];

			// Create direct child nodes of root node
			for (auto& childUUID : hierarchy.m_Children)
			{
				CreateEntity(scene, childUUID, m_RootEntityUUID);
			}
		}

		return root;
	}

	void Prefab::CreateEntity(Scene* scene, UUID dataID, UUID parent)
	{
		// Get the name of the entity
		std::string name = "";
		auto data = m_EntityDataMap[dataID];
		if ((*data)["TagComponent"])
		{
			std::string name = (*data)["TagComponent"]["Tag"].as<std::string>();
		}

		Entity entity = scene->CreateEntity(name);
		BuildComponents(entity, data);

		if (entity.HasComponent<HierarchyComponent>())
		{
			HierarchyComponent& hierarchy = entity.GetComponent<HierarchyComponent>();
			hierarchy.m_Children = m_HierarchyMap[dataID];
			hierarchy.m_Parent = parent;

			// Create direct child nodes of root node
			for (auto& childUUID : hierarchy.m_Children)
			{
				CreateEntity(scene, childUUID, dataID);
			}
		}
	}

	void Prefab::BuildComponents(Entity entity, std::shared_ptr<YAML::Node> data)
	{
		YAML::Node node = *data;

		BUILD_ENTITY_COMPONENT(entity, QuadComponent, node);
		BUILD_ENTITY_COMPONENT(entity, HierarchyComponent, node);
		BUILD_ENTITY_COMPONENT(entity, CircleComponent, node);
		BUILD_ENTITY_COMPONENT(entity, SpriteComponent, node);
		BUILD_ENTITY_COMPONENT(entity, SortingGroupComponent, node);
		BUILD_ENTITY_COMPONENT(entity, AnimatedSpriteComponent, node);
		if (m_Data["ScriptComponent"])
		{
			ScriptComponent& scriptComponent = entity.AddComponent<ScriptComponent>(ScriptComponent(node["ScriptComponent"]));
			scriptComponent.PopulateScriptInstanceData(node["ScriptComponent"]);
		}
		BUILD_ENTITY_COMPONENT(entity, CameraComponent, node);
		BUILD_ENTITY_COMPONENT(entity, RigidBody2DComponent, node);
		BUILD_ENTITY_COMPONENT(entity, BoxCollider2DComponent, node);

		// Only prefabs will have PrefabComponent as a way to identify them easily
		if (!entity.HasComponent<PrefabComponent>())
		{
			PrefabComponent& prefabComponent = entity.AddComponent<PrefabComponent>(m_Handle);
			prefabComponent.m_PrefabEntityID = node["UUIDComponent"]["UUID"].as<UUIDType>();
		}
	}

	void Prefab::UpdateComponents(Entity entity)
	{
		// Save the current entity position in order to restore it after update.
		// Provided that the entity has QuadComponent
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		if (entity.HasComponent<QuadComponent>())
		{
			QuadComponent& quad = entity.GetComponent<QuadComponent>();
			position = quad.m_Position;
		}

		REMOVE_ENTITY_COMPONENT(entity, HierarchyComponent);
		REMOVE_ENTITY_COMPONENT(entity, QuadComponent);
		REMOVE_ENTITY_COMPONENT(entity, CircleComponent);
		REMOVE_ENTITY_COMPONENT(entity, SpriteComponent);
		REMOVE_ENTITY_COMPONENT(entity, SortingGroupComponent);
		REMOVE_ENTITY_COMPONENT(entity, AnimatedSpriteComponent);
		REMOVE_ENTITY_COMPONENT(entity, ScriptComponent);
		REMOVE_ENTITY_COMPONENT(entity, CameraComponent);
		REMOVE_ENTITY_COMPONENT(entity, RigidBody2DComponent);
		REMOVE_ENTITY_COMPONENT(entity, BoxCollider2DComponent);

		UUID prefabDataUUID = entity.GetComponent<PrefabComponent>().m_PrefabEntityID;
		BuildComponents(entity, m_EntityDataMap[prefabDataUUID]);

		// Restore the entity's position
		if (entity.HasComponent<QuadComponent>())
		{
			QuadComponent& quad = entity.GetComponent<QuadComponent>();
			quad.m_Position = position;
		}
	}

	std::shared_ptr<Prefab> Prefab::Create(const std::string& name)
	{
		return std::make_shared<Prefab>(name);
	}

	std::shared_ptr<Prefab> Prefab::Create(const std::string& name, const FilePath& filePath)
	{
		YAML::Node data = YAML::LoadFile(filePath);
		if (data.IsDefined())
		{
			return std::make_shared<Prefab>(name, data);
		}

		return nullptr;
	}

	void Prefab::SerializeEntityList(Entity rootEntity, YAML::Emitter& out)
	{
		m_RootEntityUUID = rootEntity.GetComponent<UUIDComponent>().m_UUID;

		out << YAML::BeginMap;
		
		out << YAML::Key << "EntityList" << YAML::Value;
		out << YAML::BeginMap;
		SerializeEntityPrefabData(rootEntity, out);
		out << YAML::EndMap;

		out << YAML::Key << "EntityHierarchy" << YAML::Value;
		out << YAML::BeginMap;
		SerializeEntityHierarchyData(rootEntity, out);
		out << YAML::EndMap;

		out << YAML::EndMap;
	}

	void Prefab::SerializeEntityPrefabData(Entity entity, YAML::Emitter& out)
	{
		out << YAML::Key << "EntityData" << YAML::Value;
		out << YAML::BeginMap;


		SERIALIZE_ENTITY_COMPONENT(entity, UUIDComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, TagComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, HierarchyComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, QuadComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, CircleComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, SpriteComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, SortingGroupComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, AnimatedSpriteComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, ScriptComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, CameraComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, RigidBody2DComponent, out);
		SERIALIZE_ENTITY_COMPONENT(entity, BoxCollider2DComponent, out);

		out << YAML::EndMap;

		// Serialize all child entities
		if (entity.HasComponent<HierarchyComponent>())
		{
			Scene* scene = entity.GetScene();
			ST_CORE_ASSERT(!scene);

			HierarchyComponent& component = entity.GetComponent<HierarchyComponent>();
			if (!component.m_Children.empty())
			{
				for (auto childUUID : component.m_Children)
				{
					Entity child = scene->GetEntityByUUID(childUUID);
					SerializeEntityPrefabData(child, out);
				}
			}
		}
	}

	void Prefab::SerializeEntityHierarchyData(Entity entity, YAML::Emitter& out)
	{
		out << YAML::Key << "EntityHierarchyData" << YAML::Value;
		out << YAML::BeginMap;

		UUIDComponent& uuid = entity.GetComponent<UUIDComponent>();

		if (entity.HasComponent<HierarchyComponent>())
		{
			out << YAML::Key << "Owner" << YAML::Value << uuid.m_UUID;

			HierarchyComponent& hierarchy = entity.GetComponent<HierarchyComponent>();
			out << YAML::Key << "Children" << YAML::Value << hierarchy.m_Children;
		}
		
		out << YAML::EndMap;

		if (entity.HasComponent<HierarchyComponent>())
		{
			Scene* scene = entity.GetScene();
			ST_CORE_ASSERT(!scene);

			HierarchyComponent& hierarchy = entity.GetComponent<HierarchyComponent>();
			if (!hierarchy.m_Children.empty())
			{
				for (auto childUUID : hierarchy.m_Children)
				{
					Entity child = scene->GetEntityByUUID(childUUID);
					SerializeEntityHierarchyData(child, out);
				}
			}
		}
	}

	void Prefab::ParseAssetData()
	{
		YAML::Node entityList = m_Data["EntityList"];
		YAML::Node entityHierarchy = m_Data["EntityHierarchy"];

		if (entityList.IsDefined() && entityHierarchy.IsDefined())
		{
			for (YAML::const_iterator it = entityList.begin(); it != entityList.end(); it++)
			{
				YAML::Node entityNode = it->second;
				if (entityNode["UUIDComponent"])
				{
					UUID uuid = entityNode["UUIDComponent"]["UUID"].as<uint64_t>();
					m_EntityDataMap[uuid] = std::make_shared<YAML::Node>(entityNode);

					// First entity is assumed to be the root node
					if (m_RootEntityUUID == 0)
						m_RootEntityUUID = uuid;
				}
				else
				{
					ST_CORE_ERROR("Prefab asset parsing error (UUID missing)! {}", m_Name);
				}
			}

			for (YAML::const_iterator it = entityHierarchy.begin(); it != entityHierarchy.end(); it++)
			{
				YAML::Node hierarchyData = it->second;
				if (hierarchyData["Owner"] && hierarchyData["Children"])
				{
					UUID owner = hierarchyData["Owner"].as<UUIDType>();
					std::vector<UUID> children;
					YAML::Node childrenData = hierarchyData["Children"];
					for (size_t i = 0; i < childrenData.size(); i++)
					{
						children.push_back(childrenData[i].as<UUIDType>());
					}
					m_HierarchyMap[owner] = children;
				}
				else
				{
					ST_CORE_ERROR("Prefab asset parsing error (hierarchy data malformed)! {}", m_Name);
				}
			}
		}
		else
		{
			ST_CORE_ERROR("Prefab asset parsing error (malformed data)! {}", m_Name);
		}
	}

}