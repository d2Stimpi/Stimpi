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

	// TODO: consider more suitable place
	// Initialized in GraphPanel.cpp
	struct NodePanelStyle
	{
		// Node header
		float m_HeaderHeight;
		ImVec2 m_HeaderTextOffset;
		ImU32 m_NodeHoverColor;

		// Node frame
		float m_NodeRounding;
		float m_SelectionThinckness;

		// Pin
		float m_PinOffset;
		float m_PinRadius;
		float m_PinThickness;
		float m_PinArrowSpacing;
		float m_PinArrowHalfHeight;
		float m_PinArrowWidth;
		float m_PinSpacing;
		float m_PinTextSpacing; // For OUT pins
		float m_PinSelectOffset; // For higher Pin select precision

		// Connection
		uint32_t m_ConnectionSegments;

		// Grid
		float m_GridStep;
	};

	extern NodePanelStyle s_Style;

	// Utility methods
	float PointDistance(ImVec2 p1, ImVec2 p2);
	void RegenerateGraphDataAfterLoad(Graph* graph);

	// Node methods
	ImVec2 GetAvailableNodeSpaceSize(Node* node);
	ImVec2 GetNodeExtraSize(Node* node);
	bool IsNodeSpaceSizeAvailable(Node* node, ImVec2 size);
	bool ResizeNodeSpace(Node* node, ImVec2 size);

	// Pin methods
	float GetPinSpaceHeight();
	float GetPinSpaceWidth();
	bool IsConnected(Pin* src, Pin* dest);
	void ConnectPinToPin(Pin* src, Pin* dest, Graph* graph);
	void BreakPinToPinConnection(PinConnection* connection, Graph* graph);
	void UpdateConnectionPoints(PinConnection* connection);
	PinConnection* FindPinToPinConnection(Pin* src, Pin* dest, Graph* graph);
	
	// Connection methods
	ImVec2 CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end);
	ImVec2 CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end);
	ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t);
	void CalculateBezierPoints(PinConnection* connection, Pin* src, Pin* dest, uint32_t segments);

}