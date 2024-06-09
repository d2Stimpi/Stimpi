#include "stpch.h"

#include "Stimpi/Log.h"
#include "Gui/Nodes/GraphPanel.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Gui/Nodes/Nodes.h"

namespace Stimpi
{
	static std::unordered_map<std::string, std::function<Node* ()>> s_NodeBuilderFunctions;
	static std::vector<std::string> s_NodeTypeList;

	void NodeBuilder::InitializeNodeList()
	{

		s_NodeBuilderFunctions = {
			{"OnCreate", []() { return OnCreateNode::CreateNode(); }},
			{"SampleEvent", []() { return SampleEventNode::CreateNode(); }},
			{"SampleMethod", []() { return SampleMethodNode::CreateNode(); }},
			{"SetPosition", []() { return SetPositionNode::CreateNode(); }}
		};

		for (auto key : s_NodeBuilderFunctions)
		{
			s_NodeTypeList.push_back(key.first);
		}
	}

	// Called to prepare the list of possible new Nodes to add for the pop-up
	std::vector<std::string> NodeBuilder::GetNodeNamesList()
	{
		std::vector<std::string> combinedList = s_NodeTypeList;
		auto graph = GraphPanel::GetGlobalActiveGraph();

		if (graph)
		{
			for (auto& var : graph->m_Variables)
			{
				std::string setNodeName = std::string("Set_").append(var->m_Text);
				std::string getNodeName = std::string("Get_").append(var->m_Text);
				combinedList.push_back(setNodeName);
				combinedList.push_back(getNodeName);
			}
		}

		return combinedList;
	}

	Node* NodeBuilder::CreateNode(NodeLayout layout, std::string title, Graph* graph)
	{
		Node* newNode = new Node();
		newNode->m_ID = graph->GenerateNodeID();
		newNode->m_Title = title;

		for (auto& item : layout)
		{
			std::shared_ptr<Pin> pin = std::make_shared<Pin>();
			pin->m_ID = graph->GeneratePinID();
			pin->m_ParentNode = newNode;
			pin->m_Type = item.m_Type;

			pin->m_Variable->m_Text = item.m_Variable.m_Text;
			pin->m_Variable->m_ValueType = item.m_Variable.m_ValueType;
			pin->m_Variable->m_Value = item.m_Variable.m_Value; // TODO: consider removing from here as generic non-var nodes might not want this data

			if (pin->m_Type == Pin::Type::INPUT || pin->m_Type == Pin::Type::FLOW_IN)
				newNode->m_InPins.emplace_back(pin);
			else
				newNode->m_OutPins.emplace_back(pin);
		}

		return newNode;
	}

	Node* NodeBuilder::CreateNodeByName(std::string name, Graph* graph)
	{
		auto builderFunction = s_NodeBuilderFunctions[name];

		// First check for VariableNodes to avoid name collision
		for (auto& var : graph->m_Variables)
		{
			if (name.find(var->m_Text) != std::string::npos)
			{
				if (name.find("Get_") == 0)
				{
					return VariableGetNode::CreateNode(var);
				}
				else if (name.find("Set_") == 0)
				{
					return VariableSetNode::CreateNode(var);
				}
				else
				{
					// TODO: Create both set and get, drag-drop gets in here
					return VariableGetNode::CreateNode(var);
					return nullptr;
				}
			}
		}

		if (builderFunction)
		{
			return builderFunction();
		}

		ST_ERROR("Trying to create unknown expression type: {}", name);
		return nullptr;
	}

}