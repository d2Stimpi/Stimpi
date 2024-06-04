#include "stpch.h"
#include "Gui/Nodes/Methods/SampleMethodNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	Node* SampleMethodNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::FLOW_IN, {Variable::ValueType::None,"In", 0}},
			{Pin::Type::INPUT, {Variable::ValueType::Int,"Value", 0}},
			{Pin::Type::FLOW_OUT, {Variable::ValueType::None, "Out", 0}}
			}, "SampleMethod", graph);

		newNode->m_Type = Node::NodeType::Method;

		newNode->m_CodeComponent = std::make_shared<SampleMethodNode>();
		return newNode;
	}

}