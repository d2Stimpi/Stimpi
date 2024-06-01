#include "stpch.h"

#include "Stimpi/Log.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Gui/Nodes/Nodes.h"

namespace Stimpi
{
	static std::unordered_map<std::string, std::function<Node* ()>> s_NodeBuilderFunctions;
	static std::vector<std::string> s_NodeTypeList;

	void NodeBuilder::InitializeNodeList()
	{

		s_NodeBuilderFunctions = {
			{"Integer", []() { return IntegerNode::CreateNode(); }},
			{"OnCreate", []() { return OnCreateNode::CreateNode(); }},
			{"SampleEvent", []() { return SampleEventNode::CreateNode(); }},
			{"SampleMethod", []() { return SampleMethodNode::CreateNode(); }}
		};

		for (auto key : s_NodeBuilderFunctions)
		{
			s_NodeTypeList.push_back(key.first);
		}
	}

	std::vector<std::string>& NodeBuilder::GetNodeNamesList()
	{
		return s_NodeTypeList;
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
			pin->m_Text = item.m_Text;
			pin->m_Type = item.m_Type;
			pin->m_ValueType = item.m_ValueType;
			pin->m_Value = item.m_DefaultValue;
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

		if (builderFunction)
		{
			return builderFunction();
		}

		ST_ERROR("Trying to create unknown expression type: {}", name);
		return nullptr;
	}

}