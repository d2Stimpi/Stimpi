#include "stpch.h"
#include "Gui/NNode/NGraphRegistry.h"

#include "Gui/NNode/NGraphSerializer.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Scene/ResourceManager.h"

#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	NGraphRegistryType s_GraphRegistry;

	void NGraphRegistry::SerializeGraphRegistry(const FilePath& filePath)
	{
		YAML::Emitter out;

		out << YAML::Block;
		out << YAML::BeginMap;
		{
			out << YAML::Key << "NodeGraphRegistry" << YAML::Value;
			out << YAML::BeginMap;
			{
				for (auto& item : s_GraphRegistry)
				{
					out << YAML::Key << "NodeGraph" << YAML::Value;
					out << YAML::BeginMap;
					{
						out << YAML::Key << "UUID" << YAML::Value << item.first;
						out << YAML::Key << "Name" << YAML::Value << item.second->m_Name;
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

		// Clear existing data before loading
		s_GraphRegistry.clear();

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

				// Register the new graph
				s_GraphRegistry[uuid] = std::make_shared<NGraph>();
				// UUID needs to be set for loading
				auto graph = s_GraphRegistry.at(uuid);
				graph->m_ID = uuid;
				// Try to read the graph data file
				LoadGraph(graph.get());
			}
			else
			{
				if (!node["Name"]) ST_CORE_ERROR("DeserializeGraphRegistry: missing graph Name!");
				if (!node["UUID"]) ST_CORE_ERROR("DeserializeGraphRegistry: missing graph UUID!");
			}
		}
	}

	void NGraphRegistry::LoadGraph(NGraph* graph)
	{
		if (graph == nullptr)
			return;

		FilePath vsFolder = Project::GetResourcesSubdir(Project::Subdir::VisualScripting);
		std::string fileName = fmt::format("{}.ngh", graph->m_ID);
		FilePath filePath = (std::filesystem::path)vsFolder / (std::filesystem::path)fileName;

		if (std::filesystem::exists(filePath))
		{
			NGraphSerializer serializer(graph);
			serializer.Deseriealize(filePath);
		}
	}

	void NGraphRegistry::SaveGraph(NGraph* graph)
	{
		if (graph == nullptr)
			return;

		FilePath vsFolder = Project::GetResourcesSubdir(Project::Subdir::VisualScripting);
		std::string fileName = fmt::format("{}.ngh", graph->m_ID);
		FilePath filePath = (std::filesystem::path)vsFolder / (std::filesystem::path)fileName;

		NGraphSerializer serializer(graph);
		serializer.Serialize(filePath);
	}

	void NGraphRegistry::PreloadExistingGraphs()
	{
		auto registryPath = Project::GetGraphsRegistryPath();
		NGraphRegistry::DeserializeGraphRegistry(registryPath);
	}

	void NGraphRegistry::RegisterGraph(std::shared_ptr<NGraph> graph)
	{
		s_GraphRegistry[graph->m_ID] = graph;
	}

	void NGraphRegistry::UnregisterGraph(const UUID& uuid)
	{
		auto found = s_GraphRegistry.find(uuid);
		if (found != s_GraphRegistry.end())
		{
			s_GraphRegistry.erase(found);
		}
	}

	std::shared_ptr<Stimpi::NGraph> NGraphRegistry::GetGraph(const UUID& uuid)
	{
		auto found = s_GraphRegistry.find(uuid);
		if (found != s_GraphRegistry.end())
			return found->second;

		return nullptr;
	}

	Stimpi::NGraphRegistryType& NGraphRegistry::GetData()
	{
		return s_GraphRegistry;
	}

}