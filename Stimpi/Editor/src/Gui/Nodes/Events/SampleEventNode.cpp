#include "stpch.h"
#include "Gui/Nodes/Events/SampleEventNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	Node* SampleEventNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::FLOW_OUT, {Variable::ValueType::None, "Trigger", 0}}
			}, "SampleEvent", graph);

		newNode->m_Type = Node::NodeType::Event;

		newNode->m_CodeComponent = std::make_shared<SampleEventNode>();
		return newNode;
	}

}