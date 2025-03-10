#include "stpch.h"
#include "Gui/NNode/NGraphRegistry.h"

#include "Gui/NNode/NGraphSerializer.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	NGraphRegistryType s_GraphRegistry;
	NGraphUUIDRegistryType s_UUIDRegistry;
	NGraphCache s_GraphCache;

	void NGraphRegistry::SerializeGraphRegistry(const FilePath& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "NodeGraphRegistry" << YAML::Value;
			out << YAML::BeginMap;
			{
				for (auto& item : s_UUIDRegistry)
				{
					out << YAML::Key << "NodeGraph" << YAML::Value;
					out << YAML::BeginMap;
					{
						out << YAML::Key << "Name" << YAML::Value << item.first;
						out << YAML::Key << "UUID" << YAML::Value << item.second;
					}
					out << YAML::EndMap;
				}
			}
			out << YAML::EndMap;
		}
		out << YAML::EndMap;

		ResourceManager::Instance()->WriteToFile(filePath.string(), out.c_str());
	}

	void NGraphRegistry::DeserializeGraphRegistry(const FilePath& filePath)
	{
		if (!std::filesystem::exists(filePath.GetPath()))
		{
			ST_CORE_INFO("First time load project, no valid NodeGraph Registry found");
			return;
		}

		YAML::Node loadData = YAML::LoadFile(filePath.string());
		YAML::Node registry = loadData["NodeGraphRegistry"];
		ST_CORE_ASSERT_MSG(!registry, "Invalid NodeGraphRegistry file!");

		for (YAML::const_iterator it = registry.begin(); it != registry.end(); it++)
		{
			YAML::Node node = it->second;
			if (node["Name"] && node["UUID"])
			{
				std::string name = node["Name"].as<std::string>();
				UUID uuid = node["UUID"].as<UUIDType>();
				s_UUIDRegistry[name] = uuid;
			}
			else
			{
				if (!node["Name"]) ST_CORE_ERROR("DeserializeGraphRegistry: missing graph Name!");
				if (!node["UUID"]) ST_CORE_ERROR("DeserializeGraphRegistry: missing graph UUID!");
			}
		}
	}

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
					// For easy graph array lookup
					s_GraphCache.push_back(graph);
				}
			}
		}
	}

	NGraphCache NGraphRegistry::GetGraphs()
	{
		return s_GraphCache;
	}

	void NGraphRegistry::RegisterGraph(std::shared_ptr<NGraph> graph)
	{
		s_GraphRegistry[graph->m_Name] = graph;
		s_UUIDRegistry[graph->m_Name] = graph->m_ID;
	}

	void NGraphRegistry::UnregisterGraph(const std::string& name)
	{
		auto found = s_GraphRegistry.find(name);
		if (found != s_GraphRegistry.end())
		{
			s_GraphRegistry.erase(found);
			// Update local graph cache
			std::remove_if(s_GraphCache.begin(), s_GraphCache.end(), [&](std::shared_ptr<NGraph>& graph) { return graph->m_Name == name; });
		}
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