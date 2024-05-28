#include "stpch.h"

#include "Stimpi/Log.h"
#include "Gui/Nodes/NodeBuilder.h"

namespace Stimpi
{
	static std::unordered_map<std::string, std::function<Node* (Graph*)>> s_NodeBuilderFunctions;
	static std::vector<std::string> s_NodeTypeList;

	// Test builder function
	static Node* CreateMethodNode(Graph* graph)
	{
		Node* newNode = NodeBuilder::CreateNode({
			{Pin::Type::INPUT, "input"},
			{Pin::Type::OUTPUT, "output"}
			}, "Default Method", graph);

		// TODO: init Expression data here

		return newNode;
	}

	void NodeBuilder::InitializeNodeList()
	{
		s_NodeBuilderFunctions = { 
			{"New node",	nullptr },
			{"Event",		nullptr },
			{"Method",		CreateMethodNode },
			{"Variable",	nullptr }
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