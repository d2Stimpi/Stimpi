#pragma once

#include "Stimpi/Asset/Asset.h"

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/UUID.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/Scene.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class Entity;
	
	/**
	 * Move Entity from hierarchy view to ContentBrowser view => prompt to create new Prefab asset from it.
	 */

	using EntityData = std::shared_ptr<YAML::Node>;
	using PrefabEntityMap = std::unordered_map<UUID, EntityData>;

	// Map of entity - child entity vector
	using HierarchyMap = std::unordered_map<UUID, std::vector<UUID>>;

	class ST_API Prefab : public Asset
	{
	public:
		Prefab() {}
		Prefab(const std::string& name) : m_Name(name) {}
		Prefab(const std::string& name, YAML::Node data);

		void Initialize(Entity entity);
		void Save(const FilePath& filePath) const;	// Consider moving to ExportAsset

		Entity CreateEntities(Scene* scene);
		Entity CreateEntity(Scene* scene, UUID dataID, UUID parent);

		void BuildComponents(Entity entity, std::shared_ptr<YAML::Node> data);
		void UpdateComponents(Entity entity);

		static std::shared_ptr<Prefab> Create(const std::string& name);
		static std::shared_ptr<Prefab> Create(const std::string& name, const FilePath& filePath);

		std::string& GetName() { return m_Name; }
		UUID GetRootEntityID() { return m_RootEntityUUID; }

		// Reading asset data
		template <typename T>
		T GetAssetDataValue(const std::string& key)
		{
			T t;
			if (m_Data[key])
			{
				t = m_Data[key].as<T>();
			}
			return t;
		}

		// Asset
		static AssetType GetTypeStatic() { return AssetType::PREFAB; }
		AssetType GetType() override { return GetTypeStatic(); }

	private:
		void SerializeEntityList(Entity rootEntity, YAML::Emitter& out);
		void SerializeEntityPrefabData(Entity entity, YAML::Emitter& out);
		void SerializeEntityHierarchyData(Entity entity, YAML::Emitter& out);

		void ParseAssetData();

	private:
		std::string m_Name = "Unnamed";

		// Full yaml prefab data
		YAML::Node m_Data;

		// Per entity data
		UUID m_RootEntityUUID = 0;
		PrefabEntityMap m_EntityDataMap;
		HierarchyMap m_HierarchyMap;
	};

}