#include "stpch.h"
#include "Gui/NNode/NGraphBuilder.h"

#include "Gui/NNode/NNodeRegistry.h"

namespace Stimpi
{
	static std::string MethodNameToString(const MethodName& methodName)
	{

		switch (methodName)
		{
#define DEFINE_NODE_NAME(name)	case MethodName::name: return #name;
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)
		default:
			return "None";
		}

		return "None";
	}


	std::shared_ptr<Stimpi::NGraph> NGraphBuilder::BuildGraph(const NGraphBuildInfo& buildInfo)
	{
		auto graph = std::make_shared<NGraph>();
		graph->m_Name = buildInfo.m_Name;

		ImVec2 pos = { 300.0f, 180.0f };

		for (auto& method : buildInfo.m_Methods)
		{
			auto node = NNodeRegistry::CreateNodeByName(MethodNameToString(method));
			node->m_Pos = pos;
			graph->m_Nodes.push_back(node);

			// Move pos for other nodes to avoid full overlap
			pos = { pos.x + 10.0f, pos.y + 10.0f };
		}

		return graph;
	}

}