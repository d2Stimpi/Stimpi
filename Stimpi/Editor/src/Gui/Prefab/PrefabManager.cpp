#include "stpch.h"
#include "Gui/Prefab/PrefabManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
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

	bool PrefabManager::IsEntityValidPrefab(Entity entity)
	{
		if (entity && entity.HasComponent<PrefabComponent>())
		{
			PrefabComponent& component = entity.GetComponent<PrefabComponent>();
			if (component.m_PrefabHandle)
			{
				auto asset = AssetManager::GetAsset<Prefab>(component.m_PrefabHandle);
				if (asset->GetType() == AssetType::PREFAB)
					return true;
			}
		}

		return false;
	}

	Entity PrefabManager::GetPrefabRootEntity(Entity entity)
	{
		if (IsEntityValidPrefab(entity))
		{
			auto scene = SceneManager::Instance()->GetActiveScene();
			if (scene)
			{
				bool found = false;
				uint32_t loopbreak = 0;
				while (!found)
				{
					if (entity.HasComponent<HierarchyComponent>())
					{
						HierarchyComponent& hierarchy = entity.GetComponent<HierarchyComponent>();
						if (!hierarchy.m_Parent)
						{
							found = true;
						}
						else
						{
							// move to next parent entity and check
							entity = scene->GetEntityByUUID(hierarchy.m_Parent);
						}
					}
					else
					{
						found = true;
					}

					if (++loopbreak >= 100)
					{
						ST_ERROR("[PrefabManager::GetPrefabRootEntity] stuck in loop - break! (entity hierarchy handling issue)");
						return {};
					}
				}

				return entity;
			}
		}

		return {};
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

	void PrefabManager::ConvertToPrefabEntity(Entity entity, AssetHandle prefabHandle, bool isRootObject)
	{
		if (!entity.HasComponent<PrefabComponent>())
		{
			PrefabComponent& prefabComponent = entity.AddComponent<PrefabComponent>(prefabHandle);
			prefabComponent.m_PrefabEntityID = entity.GetComponent<UUIDComponent>().m_UUID;
			prefabComponent.m_IsRootObject = isRootObject;

			if (entity.HasComponent<HierarchyComponent>())
			{
				Scene* scene = entity.GetScene();
				ST_CORE_ASSERT(!scene);

				HierarchyComponent& hierarchyComponent = entity.GetComponent<HierarchyComponent>();
				for (auto childUUID : hierarchyComponent.m_Children)
				{
					Entity child = scene->GetEntityByUUID(childUUID);
					ConvertToPrefabEntity(entity, prefabHandle, isRootObject);
				}
			}
		}
	}

}