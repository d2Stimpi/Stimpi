#include "stpch.h"
#include "Gui/Nodes/Variables/VariableGetNode.h"

#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Stimpi/Log.h"

namespace Stimpi
{

	VariableGetNode::VariableGetNode(Node* node)
		: m_Node(node)
	{

	}

	VariableGetNode::~VariableGetNode()
	{
		auto& rem = m_Node->m_OutPins.front();
		auto& var = rem->m_Variable;
		auto& attached = var->m_AttachedToPins;
		
		// Remove the link to variable->m_AttachedToPins since multiple nodes can link to single variable (duplicate var nodes)
		attached.erase(std::find_if(attached.begin(), attached.end(), [&rem](auto pin)
			{
				return pin->m_ID == rem->m_ID;
			}));
	}

	std::string VariableGetNode::GetValueName(Node* node)
	{
		if (!node->m_OutPins.empty())
		{
			auto outPin = node->m_OutPins[0];
			return VariableValueTypeToString(outPin->m_Variable->m_ValueType);
		}

		return "";
	}

	Stimpi::Node* VariableGetNode::CreateNode(std::shared_ptr<Variable> var)
	{
		auto graph = GraphPanel::GetGlobalActiveGraph();
		ST_ASSERT(!graph, "No Active Graph found!");

		Node* newNode = new Node();
		newNode->m_ID = graph->GenerateNodeID();
		newNode->m_Title = "";
		newNode->m_Type = Node::NodeType::Variable;
		newNode->m_HasHeader = false;
		newNode->m_CodeComponent = std::make_shared<VariableGetNode>(newNode);
		
		std::shared_ptr<Pin> pin = std::make_shared<Pin>();
		pin->m_ID = graph->GeneratePinID();
		pin->m_ParentNode = newNode;
		pin->m_Type = Pin::Type::OUTPUT;
		pin->m_Variable = var;
		
		newNode->m_OutPins.emplace_back(pin);
		if (newNode->m_Type == Node::NodeType::Variable)
		{
			pin->m_Variable->m_AttachedToPins.push_back(pin);
		}

		return newNode;
	}

}