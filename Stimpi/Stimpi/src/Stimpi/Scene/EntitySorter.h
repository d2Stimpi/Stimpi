#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"

namespace Stimpi
{
	class Entity;

	struct EntitySortData
	{
		uint32_t m_EntityID;
		uint32_t m_SortOrder;
	};

	struct EntityLayerGroup
	{
		std::vector<EntitySortData> m_Entities;
	};

	class ST_API EntitySorter
	{
	public:

		// Called when SortingGroupComponent is created
		void SortEntityByLayer(EntitySortData data, const std::string& layerName);
		void RemoveLayerSortedEntity(uint32_t entityID, const std::string& layerName);
		void SortingLayerRemoved(const std::string& layerName);

		void SortEntityByAxis(uint32_t entityID, float value);
		void RemoveAxisSortedEntity(uint32_t entityID);
		void ResetAxisSortedEntites();

		std::unordered_map<std::string, EntityLayerGroup>& GetEntityGroups();
		std::vector<uint32_t>& GetAxisSortedEntites();
	private:
		std::unordered_map<std::string, EntityLayerGroup> m_EntityGroups = {};
		std::vector<uint32_t> m_AxisSortedEntites = {};

		friend class Scene;
	};
}