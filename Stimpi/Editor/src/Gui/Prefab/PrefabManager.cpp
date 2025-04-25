#include "stpch.h"
#include "Gui/Prefab/PrefabManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/AssetManager.h"

namespace Stimpi
{

	Entity PrefabManager::InstantiatePrefab(AssetHandle prefabHandle)
	{
		if (prefabHandle)
		{
			auto scene = SceneManager::Instance()->GetActiveScene();
			if (scene)
			{
				return scene->CreateEntity(prefabHandle);
			}
		}

		return Entity();
	}

	void PrefabManager::OnPrefabAssetReload(std::shared_ptr<Asset> asset)
	{
		if (asset->GetType() == AssetType::PREFAB)
		{
			AssetHandle prefabHandle = asset->m_Handle;
			// Update prefab instances
			ST_INFO("PrefabManager: Prefab asset reloaded {}", asset->m_Handle);

			auto scene = SceneManager::Instance()->GetActiveScene();
			if (scene)
			{
				std::shared_ptr<Prefab> prefab = AssetManager::GetAsset<Prefab>(prefabHandle);
				auto entities = scene->FindAllPrefabEntities(prefabHandle);
				for (auto& entity : entities)
				{
					prefab->UpdateComponents(entity);
				}
			}
		}

	}

}