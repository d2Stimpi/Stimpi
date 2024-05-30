#include "stpch.h"

#include "Stimpi/Log.h"
#include "Gui/Nodes/NodeBuilder.h"
#include "Gui/Nodes/NodeNames.h"

namespace Stimpi
{
	static std::unordered_map<std::string, std::function<Node* (Graph*)>> s_NodeBuilderFunctions;
	static std::vector<std::string> s_NodeTypeList;

	static Node* CreateOnCreateNode(Graph* graph)
	{
		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::OUTPUT, Pin::ValueType::Flow, "Out"}
			}, "OnCreate", graph);

		newNode->m_Type = Node::NodeType::Create;

		return newNode;
	}

	static Node* CreateEvnetNode(Graph* graph)
	{
		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::OUTPUT, Pin::ValueType::Flow, "Trigger"}
			}, "Event", graph);

		newNode->m_Type = Node::NodeType::Event;

		return newNode;
	}

	static Node* CreateMethodNode(Graph* graph)
	{
		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::INPUT, Pin::ValueType::Flow,"In"},
			{Pin::Type::INPUT, Pin::ValueType::Int,"Value"},
			{Pin::Type::OUTPUT, Pin::ValueType::Flow, "Out"}
			}, "Method", graph);

		newNode->m_Type = Node::NodeType::Method;

		return newNode;
	}

	static Node* CreateVariableNode(Graph* graph)
	{
		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::OUTPUT, Pin::ValueType::Int, "Variable"}
			}, "Variable", graph);

		newNode->m_Type = Node::NodeType::Variable;

		return newNode;
	}

	void NodeBuilder::InitializeNodeList()
	{
		s_NodeBuilderFunctions = { 
			{ ONCREATE_NODE,	CreateOnCreateNode },
			{ EVENT_NODE,		CreateEvnetNode },
			{ METHOD_NODE,		CreateMethodNode },
			{ VARIABLE_NODE,	CreateVariableNode }
		};

		// Build node type list
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
		newNode->m_Size = { 250.0f, 100.0f };
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
			return builderFunction(graph);
		}

		ST_ERROR("Trying to create unknown expression type: {}", name);
		return nullptr;
	}

}