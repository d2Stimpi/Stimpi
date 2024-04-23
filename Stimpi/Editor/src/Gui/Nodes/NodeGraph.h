#pragma once

#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	class NodeGraph
	{
	public:
		NodeGraph() = default;
		NodeGraph(const std::string& name) : m_Name(name) {}

		//bool IsNodeSelected(Node* node);

	private:
		std::string m_Name = "Graph";
		uint32_t m_NextNodeID = 0;
		uint32_t m_NextPinID = 0;

		std::vector<std::shared_ptr<Node>> m_Nodes;
		std::vector<std::shared_ptr<PinConnection>> m_PinConnections;

		// ImGui show tab item flag
		bool m_Show = true;

		// Internal, not serialized data

		// For easier Pin search
		std::vector<std::shared_ptr<Pin>> m_Pins;
	};
}