#pragma once

#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	using NGraphRegistryType = std::unordered_map<UUID, std::shared_ptr<NGraph>>;

	class NGraphRegistry
	{
	public:
		static void SerializeGraphRegistry(const FilePath& filePath);
		static void DeserializeGraphRegistry(const FilePath& filePath);
		static void SaveGraph(NGraph* graph);
		static void LoadGraph(NGraph* graph);

		static void PreloadExistingGraphs();

		static void RegisterGraph(std::shared_ptr<NGraph> graph);
		static void UnregisterGraph(const UUID& uuid);

		static std::shared_ptr<NGraph> GetGraph(const UUID& uuid);
		static NGraphRegistryType& GetData();
	};
}