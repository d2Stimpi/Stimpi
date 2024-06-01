#include "stpch.h"
#include "Gui/Nodes/Variables/IntegerNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	IntegerNode::IntegerNode()
	{
		
	}

	std::string IntegerNode::GetValueName(Node* node)
	{
		if (!node->m_OutPins.empty())
		{
			auto outPin = node->m_OutPins[0];
			return PinValueTypeToString(outPin->m_ValueType);
		}

		return "";
	}

	Stimpi::Node* IntegerNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::OUTPUT, Pin::ValueType::Int, "Integer", 0}
			}, "Integer", graph);

		newNode->m_Type = Node::NodeType::Variable;
		newNode->m_HasHeader = false;
		
		newNode->m_CodeComponent = std::make_shared<IntegerNode>();
		return newNode;
	}

}