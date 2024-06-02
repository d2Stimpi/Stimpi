#pragma once

#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	

	class NodeBuilder
	{
	public:
		static void InitializeNodeList();

		static std::vector<std::string> GetNodeNamesList();

		static Node* CreateNode(NodeLayout layout, std::string title, Graph* graph);
		static Node* CreateNodeByName(std::string name, Graph* graph);
	};
}