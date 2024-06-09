#include "stpch.h"
#include "Gui/Nodes/Methods/SetPositionNode.h"

#include "Gui/CodeGen/CodeWriter.h"
#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	Stimpi::Node* SetPositionNode::CreateNode()
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::FLOW_IN, {Variable::ValueType::None,"In", 0}},
			{Pin::Type::INPUT, {Variable::ValueType::Vector2,"Position", glm::vec2{0.0f, 0.0f}}},
			{Pin::Type::FLOW_OUT, {Variable::ValueType::None, "Out", 0}}
			}, "Set Position", graph);

		newNode->m_Type = Node::NodeType::Method;
		newNode->m_CodeComponent = std::make_shared<SetPositionNode>();

		return newNode;
	}

	void SetPositionNode::GenerateExpression(Node* node, CodeWriter* writer)
	{
		// Check if Input pin has data connected
		if (node->m_InPins[0]->m_Connected)
		{
			// Check for valid pin and get the variable attached
			auto pin = node->m_InPins[1]->m_ConnectedPins.front();
			if (pin->m_Variable)
			{
				*writer << "QuadComponent quad = GetComponent<QuadComponent>();" << std::endl;
				*writer << "if (quad != null)" << std::endl;
				*writer << "{" << std::endl;
				*writer << "    quad.Position = " << pin->m_Variable->m_Text << ";" << std::endl;
				*writer << "}" << std::endl;
			}
		}
	}

}