#include "stpch.h"
#include "Gui/NNode/NGraphRegistry.h"

namespace Stimpi
{
	NGraphRegistryType s_GraphRegistry;
	NGraphUUIDRegistryType s_UUIDRegistry;

	void NGraphRegistry::RegisterGraph(std::shared_ptr<NGraph> graph)
	{
		s_GraphRegistry[graph->m_Name] = graph;
		s_UUIDRegistry[graph->m_Name] = graph->m_ID;
	}

	std::shared_ptr<Stimpi::NGraph> NGraphRegistry::GetGraph(const std::string& name)
	{
		return nullptr;
	}

	std::shared_ptr<Stimpi::NGraph> NGraphRegistry::GetGraph(const UUID& id)
	{
		return nullptr;
	}



}