#pragma once

#include "Stimpi/Asset/Asset.h"

#include "Stimpi/Core/Core.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/Scene.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	class Entity;
	
	/**
	 * Move Entity from hierarchy view to ContentBrowser view => prompt to create new Prefab asset from it.
	 */

	class ST_API Prefab : public Asset
	{
	public:
		Prefab() = default;
		Prefab(YAML::Node data);
		Prefab(Entity entity);

		void Initialize(Entity entity);
		void Save(const FilePath& filePath);	// Consider moving to ExportAsset

		void BuildComponents(Entity entity);

		static std::shared_ptr<Prefab> Create();
		static std::shared_ptr<Prefab> Create(Entity entity);
		static std::shared_ptr<Prefab> Create(const FilePath& filePath);

		// Asset
		static AssetType GetTypeStatic() { return AssetType::PREFAB; }
		AssetType GetType() override { return GetTypeStatic(); }
	private:
		YAML::Node m_Data;
	};
}