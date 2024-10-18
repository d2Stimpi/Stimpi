#include "stpch.h"
#include "Stimpi/Scene/EntitySorter.h"

#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/SceneManager.h"

namespace Stimpi
{

	void EntitySorter::SortEntityByLayer(EntitySortData data, const std::string& layerName)
	{
		auto& vector = m_EntityGroups[layerName].m_Entities;
		vector.insert(
			std::upper_bound(vector.begin(), vector.end(), data.m_SortOrder, 
				[](uint32_t layerOrder, const EntitySortData& item) {
					return layerOrder < item.m_SortOrder;
				}), data
		);
	}

	void EntitySorter::RemoveLayerSortedEntity(uint32_t entityID, const std::string& layerName)
	{
		if (m_EntityGroups.find(layerName) != m_EntityGroups.end())
		{
			auto& vector = m_EntityGroups.at(layerName).m_Entities;
			for (auto it = vector.begin(); it != vector.end(); it++)
			{
				auto entitySortData = *it;
				if (entitySortData.m_EntityID == entityID)
				{
					vector.erase(it);
					break;
				}
			}
		}
	}

	void EntitySorter::SortingLayerRemoved(const std::string& layerName)
	{
		if (m_EntityGroups.find(layerName) != m_EntityGroups.end())
		{
			m_EntityGroups.erase(layerName);
		}
	}

	void EntitySorter::SortQuadEntityByAxis(uint32_t entityID, RenderingOrderAxis axis)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		Entity entity = { (entt::entity)entityID, scene };
		float value = entity.GetComponent<QuadComponent>().m_Position[(uint32_t)axis - 1];

		if (std::find(m_AxisSortedEntites.begin(), m_AxisSortedEntites.end(), entityID) == m_AxisSortedEntites.end())
		{
			auto comp = [&axis, &scene](float v, auto entityID)
			{
				Entity entity = { (entt::entity)entityID, scene };
				float cmpVal = entity.GetComponent<QuadComponent>().m_Position[(uint32_t)axis - 1];
				return v > cmpVal;
			};

			m_AxisSortedEntites.insert(
				std::upper_bound(m_AxisSortedEntites.begin(), m_AxisSortedEntites.end(), value, comp),
				entityID
			);
		}
	}

	void EntitySorter::SortCircleEntityByAxis(uint32_t entityID, RenderingOrderAxis axis)
	{
		auto scene = SceneManager::Instance()->GetActiveScene();
		Entity entity = { (entt::entity)entityID, scene };
		float value = entity.GetComponent<CircleComponent>().m_Position[(uint32_t)axis - 1];

		if (std::find(m_AxisSortedEntites.begin(), m_AxisSortedEntites.end(), entityID) == m_AxisSortedEntites.end())
		{
			auto comp = [&axis, &scene](float v, auto entityID)
			{
				Entity entity = { (entt::entity)entityID, scene };
				float cmpVal = entity.GetComponent<CircleComponent>().m_Position[(uint32_t)axis - 1];
				return v > cmpVal;
			};

			m_AxisSortedEntites.insert(
				std::upper_bound(m_AxisSortedEntites.begin(), m_AxisSortedEntites.end(), value, comp),
				entityID
			);
		}
	}

	void EntitySorter::RemoveAxisSortedEntity(uint32_t entityID)
	{
		for (auto it = m_AxisSortedEntites.begin(); it != m_AxisSortedEntites.end(); it++)
		{
			if (*it == entityID)
			{
				m_AxisSortedEntites.erase(it);
				break;
			}
		}
	}

	void EntitySorter::ResetAxisSortedEntites()
	{
		m_AxisSortedEntites.clear();
	}

	std::unordered_map<std::string, Stimpi::EntityLayerGroup>& EntitySorter::GetEntityGroups()
	{
		return m_EntityGroups;
	}

	std::vector<uint32_t>& EntitySorter::GetAxisSortedEntites()
	{
		return m_AxisSortedEntites;
	}

	void EntitySorter::ClearAllSortingData()
	{
		for (auto& item : m_EntityGroups)
		{
			auto& group = item.second;
			group.m_Entities.clear();
		}
		m_EntityGroups.clear();
		m_AxisSortedEntites.clear();
	}

}