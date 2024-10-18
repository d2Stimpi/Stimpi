#include "stpch.h"
#include "Gui/Nodes/Variables/VariableSetNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	VariableSetNode::VariableSetNode(Node* node)
		: m_Node(node)
	{

	}

	VariableSetNode::~VariableSetNode()
	{
		auto& rem = m_Node->m_OutPins.front();
		auto& var = rem->m_Variable;
		auto& attached = var->m_AttachedToPins;

		// Remove the link to variable->m_AttachedToPins since multiple nodes can link to single variable (duplicate var nodes)
		if (!attached.empty())
		{
			attached.erase(std::find_if(attached.begin(), attached.end(), [&rem](auto pin)
			{
				return pin->m_ID == rem->m_ID;
			}));
		}
	}

	std::string VariableSetNode::GetValueName(Node* node)
	{
		if (!node->m_OutPins.empty())
		{
			auto outPin = node->m_OutPins[0];
			return VariableValueTypeToString(outPin->m_Variable->m_ValueType);
		}

		return "";
	}

	Stimpi::Node* VariableSetNode::CreateNode(std::shared_ptr<Variable> var)
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = new Node();
		newNode->m_ID = graph->GenerateNodeID();
		newNode->m_Title = "SET";
		newNode->m_Type = Node::NodeType::Method;
		newNode->m_HasHeader = true;
		newNode->m_CodeComponent = std::make_shared<VariableSetNode>(newNode);

		std::shared_ptr<Pin> outFlowPin = std::make_shared<Pin>();
		outFlowPin->m_Type = Pin::Type::FLOW_OUT;
		outFlowPin->m_ID = graph->GeneratePinID();
		outFlowPin->m_ParentNode = newNode;
		newNode->m_OutPins.push_back(outFlowPin);

		std::shared_ptr<Pin> outPin = std::make_shared<Pin>();
		outPin->m_Type = Pin::Type::OUTPUT;
		outPin->m_ID = graph->GeneratePinID();
		outPin->m_ParentNode = newNode;
		outPin->m_Variable = var;
		newNode->m_OutPins.push_back(outPin);
		outPin->m_Variable->m_AttachedToPins.push_back(outPin);

		std::shared_ptr<Pin> inFlowPin = std::make_shared<Pin>();
		inFlowPin->m_Type = Pin::Type::FLOW_IN;
		inFlowPin->m_ID = graph->GeneratePinID();
		inFlowPin->m_ParentNode = newNode;
		newNode->m_InPins.push_back(inFlowPin);

		std::shared_ptr<Pin> inPin = std::make_shared<Pin>();
		inPin->m_Type = Pin::Type::INPUT;
		inPin->m_ID = graph->GeneratePinID();
		inPin->m_ParentNode = newNode;
		inPin->m_Variable = var;
		newNode->m_InPins.push_back(inPin);
		inPin->m_Variable->m_AttachedToPins.push_back(inPin);

		return newNode;
	}

}