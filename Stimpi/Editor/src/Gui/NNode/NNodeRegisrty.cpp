#include "stpch.h"
#include "Gui/NNode/NNodeRegistry.h"

#define ST_REGISTER_NODE(Name, FuncName) s_NodeNames.emplace_back(Name); s_NodeRegistry[Name] = []() { return FuncName(Name); };

namespace Stimpi
{
	using NNodeCtor = std::function<std::shared_ptr<NNode>()>;
	using NNodeRegistryType = std::unordered_map<std::string, NNodeCtor>;

	std::vector<std::string> s_NodeNames;
	NNodeRegistryType s_NodeRegistry;

	/* Node construction methods begin */

	static std::shared_ptr<NNode> SampleGetter(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title,	NNode::Type::Getter);
		auto pin = std::make_shared<NPin>(node, NPin::Type::In, "Input");
		node->m_InPins.push_back(pin);

		return node;
	}

	static std::shared_ptr<NNode> SampleNode(const std::string& title)
	{
		auto node = std::make_shared<NNode>(title, NNode::Type::Modifier);
		auto pinIn = std::make_shared<NPin>(node, NPin::Type::In, "Data");
		auto pinOut = std::make_shared<NPin>(node, NPin::Type::Out, "Output");
		node->m_InPins.push_back(pinIn);
		node->m_OutPins.push_back(pinOut);

		return node;
	}


	/* Node construction methods end */

	void NNodeRegistry::InitializeNodeRegisrty()
	{
		ST_REGISTER_NODE("Sample Getter", SampleGetter);
		ST_REGISTER_NODE("Sample Node", SampleNode);
	}

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