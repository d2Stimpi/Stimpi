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
			{"SampleMethod", []() { return SampleMethodNode::CreateNode(); }}
		};

		for (auto key : s_NodeBuilderFunctions)
		{
			s_NodeTypeList.push_back(key.first);
		}
	}

	std::vector<std::string> NodeBuilder::GetNodeNamesList()
	{
		std::vector<std::string> compinedLinst = s_NodeTypeList;
		auto graph = GraphPanel::GetGlobalActiveGraph();

		if (graph)
		{
			for (auto& var : graph->m_Variables)
			{
				compinedLinst.push_back(var->m_Text);
			}
		}

		return compinedLinst;
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
			pin->m_Variable->m_Value = item.m_Variable.m_Value;
			if (newNode->m_Type == Node::NodeType::Variable)
			{
				pin->m_Variable->m_AttachedToPins.push_back(pin);
			}

			if (pin->m_Type == Pin::Type::INPUT)
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
			if (name == var->m_Text)
			{
				return VariableGetNode::CreateNode(var);
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