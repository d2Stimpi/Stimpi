#pragma once

#include "Gui/NNode/NGraph.h"

/**
 * TODO: GraphRegistry.txt
 *  - graph [name : UUID] pairs
 * 
 *  Saving graphs should then be UUID.egh and UUID.ngh
 *  This way we can have graphs with the same name.
 *  Update logic that opens graphs in SceneHierarchyWindow.cpp 
 */

namespace Stimpi
{
	using NGraphRegistryType = std::unordered_map<std::string, std::shared_ptr<NGraph>>;
	using NGraphUUIDRegistryType = std::unordered_map<std::string, UUID>;
	using NGraphCache = std::vector<std::shared_ptr<NGraph>>;

	class NGraphRegistry
	{
	public:
		static void SerializeGraphRegistry(const FilePath& filePath);
		static void DeserializeGraphRegistry(const FilePath& filePath);

		static void PreloadExistingGraphs(); // TODO: use load registry instead
		static NGraphCache GetGraphs();

		static void RegisterGraph(std::shared_ptr<NGraph> graph);
		static void UnregisterGraph(const std::string& name);

		static std::shared_ptr<NGraph> GetGraph(const std::string& name);
		static UUID GetUUID(const std::string& name);
	};
}