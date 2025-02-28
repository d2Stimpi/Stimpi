#include "stpch.h"
#include "Gui/NNode/NGraphRegistry.h"

#include "Gui/NNode/NGraphSerializer.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{
	NGraphRegistryType s_GraphRegistry;
	NGraphUUIDRegistryType s_UUIDRegistry;

	void NGraphRegistry::PreloadExistingGraphs()
	{
		FilePath vsFolder = Project::GetResourcesSubdir(Project::Subdir::VisualScripting);

		for (auto& directoryEntry : std::filesystem::directory_iterator(vsFolder))
		{
			const auto& path = directoryEntry.path();
			if (path.extension() == ".ngh")
			{
				auto graph = std::make_shared<NGraph>();
				NGraphSerializer serializer(graph.get());
				if (serializer.Deseriealize(path))
				{
					RegisterGraph(graph);
				}
			}
		}
	}

	void NGraphRegistry::RegisterGraph(std::shared_ptr<NGraph> graph)
	{
		s_GraphRegistry[graph->m_Name] = graph;
		s_UUIDRegistry[graph->m_Name] = graph->m_ID;
	}

	std::shared_ptr<Stimpi::NGraph> NGraphRegistry::GetGraph(const std::string& name)
	{
		auto found = s_GraphRegistry.find(name);
		if (found != s_GraphRegistry.end())
			return found->second;

		return nullptr;
	}

	UUID NGraphRegistry::GetUUID(const std::string& name)
	{
		auto found = s_UUIDRegistry.find(name);
		if (found != s_UUIDRegistry.end())
			return found->second;

		return 0;
	}

}