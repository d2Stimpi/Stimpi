#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	struct Node;
	struct PinConnection;

	struct Pin
	{
		enum class Type { INPUT = 0, OUTPUT };

		Node* m_ParentNode;	// Owner of the pin
		uint32_t m_ID;	    // Pin ID

		bool m_Connected = false;
		bool m_SingleConnection = false;
		std::vector<Pin*> m_ConnectedPins;

		std::string m_Text;
		Type m_Type;

		// For handling selection - global position
		ImVec2 m_Pos = { 0.0f, 0.0f };
	};

	struct Node
	{
		ImVec2 m_Pos;
		ImVec2 m_Size;
		uint32_t m_ID = 0;

		std::string m_Title;

		// List of Pins
		std::vector<std::shared_ptr<Pin>> m_InPins;
		std::vector<std::shared_ptr<Pin>> m_OutPins;

		Node() = default;
		Node(const Node&) = default;
		Node(const ImVec2& pos) : m_Pos(pos) {}

		static bool IsValid(Node& node) { return node.m_ID != 0; }
	};

	struct PinConnection
	{
		Pin* m_SourcePin;
		Pin* m_DestinationPin;

		std::vector<ImVec2> m_BezierLinePoints;

		PinConnection() = default;
		PinConnection(Pin* src, Pin* dest) : m_SourcePin(src), m_DestinationPin(dest) {}
	};

	struct Graph
	{
		//uint32_t m_ID = 0;
		uint32_t m_NextNodeID = 0;
		uint32_t m_NextPinID = 0;
		std::string m_Name = "Graph";

		std::vector<std::shared_ptr<Node>> m_Nodes;
		std::vector<std::shared_ptr<PinConnection>> m_PinConnections;
		// For easier Pin search
		std::vector<std::shared_ptr<Pin>> m_Pins;

		// ImGui show tab item flag
		bool m_Show = true;

		Graph() = default;
		Graph(const Graph&) = default;
		Graph(const std::string& name) : m_Name(name) {}
	};
}