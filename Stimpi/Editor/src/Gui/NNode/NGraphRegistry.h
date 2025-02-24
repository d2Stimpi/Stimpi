#pragma once

#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	using NGraphRegistryType = std::unordered_map<std::string, std::shared_ptr<NGraph>>;
	using NGraphUUIDRegistryType = std::unordered_map<std::string, UUID>;

	class NGraphRegistry
	{
	public:
		static void RegisterGraph(std::shared_ptr<NGraph> graph);
		static std::shared_ptr<NGraph> GetGraph(const std::string& name);
		static std::shared_ptr<NGraph> GetGraph(const UUID& id);
	};
}