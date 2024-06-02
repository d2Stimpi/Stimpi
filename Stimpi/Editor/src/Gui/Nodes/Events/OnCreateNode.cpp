#include "stpch.h"
#include "Gui/Nodes/Events/OnCreateNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	Stimpi::Node* OnCreateNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::OUTPUT, {Variable::ValueType::Flow, "Out", false}}
			}, "OnCreate", graph);

		newNode->m_Type = Node::NodeType::Create;

		newNode->m_CodeComponent = std::make_shared<OnCreateNode>();
		return newNode;
	}

}