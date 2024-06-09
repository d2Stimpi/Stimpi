#include "stpch.h"
#include "Gui/Nodes/Events/OnCreateNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{
	static std::string s_Name = "OnCreate";

	Stimpi::Node* OnCreateNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::FLOW_OUT, {Variable::ValueType::None, "Out", 0}}
			}, s_Name, graph);

		newNode->m_Type = Node::NodeType::Create;

		newNode->m_CodeComponent = std::make_shared<OnCreateNode>();
		return newNode;
	}

	std::string OnCreateNode::GetName()
	{
		return s_Name;
	}

}