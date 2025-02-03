#include "stpch.h"
#include "Gui/NNode/NNodeRegistry.h"

namespace Stimpi
{
	using NNodeCtor = std::function<std::shared_ptr<NNode>()>;
	using NNodeRegistryType = std::unordered_map<std::string, NNodeCtor>;

	std::vector<std::string> s_NodeNames = { "Sample Getter", "Sample Node" };

	NNodeRegistryType s_NodeRegistry = 
	{
		{s_NodeNames[0], []() { return std::make_shared<NNode>(); }},
		{s_NodeNames[1], []() { return std::make_shared<NNode>(); }}
	};

	std::vector<std::string> NNodeRegistry::GetNodeNamesList()
	{
		return s_NodeNames;
	}

	std::shared_ptr<Stimpi::NNode> NNodeRegistry::CreateNodeByName(const std::string& name)
	{
		std::shared_ptr<NNode> node = nullptr;

		auto found = s_NodeRegistry.find(name);
		if (found != s_NodeRegistry.end())
			node = found->second();

		return node;
	}
}