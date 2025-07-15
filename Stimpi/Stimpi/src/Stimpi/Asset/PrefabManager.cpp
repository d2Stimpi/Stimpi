#include "stpch.h"
#include "Stimpi/Asset/PrefabManager.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/EntityManager.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/AssetManager.h"

namespace Stimpi
{

	Entity PrefabManager::InstantiatePrefab(AssetHandle prefabHandle, glm::vec3 position)
	{
		if (prefabHandle)
		{
			auto scene = SceneManager::Instance()->GetActiveScene();
			if (scene)
			{
				Entity rootEntity = scene->CreateEntity(prefabHandle);
				if (rootEntity)
				{
					if (rootEntity.HasComponent<QuadComponent>())
					{
						QuadComponent& quad = rootEntity.GetComponent<QuadComponent>();
						// Center the entity to the passed position.
						EntityManager::Translate(rootEntity, { position.x - quad.m_Position.x, position.y - quad.m_Position.y, 0.0f });
					}
					else
					{
						// In case the root entity is used only for grouping multiple pieces.
						// Make sure to move all child entities.
						EntityManager::Translate(rootEntity, { position.x, position.y, 0.0f });
					}
				}
				return rootEntity;
			}
		}

		return {};
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

	std::vector<Entity> PrefabManager::GetAllPrefabEntities(const AssetHandle& prefabHandle)
	{
		std::vector<Entity> entities;

		auto scene = SceneManager::Instance()->GetActiveScene();
		if (scene)
		{
			auto view = scene->m_Registry.view<PrefabComponent>();
			for (auto& entity : view)
			{
				const PrefabComponent& prefab = view.get<PrefabComponent>(entity);
				if (prefab.m_PrefabHandle == prefabHandle)
					entities.emplace_back(entity, scene);
			}
		}
		return entities;
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
				auto entities = GetAllPrefabEntities(prefabHandle);
				for (auto& entity : entities)
				{
					prefab->UpdateComponents(entity);
				}
			}
		}
	}

}