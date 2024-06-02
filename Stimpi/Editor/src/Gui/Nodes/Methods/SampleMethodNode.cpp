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
			{Pin::Type::INPUT, {Variable::ValueType::Flow,"In", false}},
			{Pin::Type::INPUT, {Variable::ValueType::Int,"Value", 0}},
			{Pin::Type::OUTPUT, {Variable::ValueType::Flow, "Out", false}}
			}, "SampleMethod", graph);

		newNode->m_Type = Node::NodeType::Method;

		newNode->m_CodeComponent = std::make_shared<SampleMethodNode>();
		return newNode;
	}

}