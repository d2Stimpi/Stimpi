#pragma once

#include "Gui/Nodes/CodeComponent.h"

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

#include "Stimpi/Log.h"

#define PIN_VALUE_TYPES_LIST	{ "Bool", "Int", "Float", "Vector2", "String" }
#define PIN_VALUE_TYPE_TO_INT(x)	((int)(x) - 2)
#define INT_TO_PIN_VALUE_TYPE(x)	((Variable::ValueType)(x + 2))

namespace Stimpi
{
	struct Pin;
	struct Node;
	struct PinConnection;

	// Adding new type 
	//	-> GraphPanel function for value input
	//  -> CodeWriter for vriting variant type
	//  -> GraphRender specific type color
	using pin_type_variant = std::variant<bool, int, float, glm::vec2, std::string>;

	// Helper for generating NewVar_x name
	static uint32_t s_VarCount = 0;
	static std::string GenNewVarName()
	{
		std::string name = fmt::format("NewVar_{}", s_VarCount);
		return name;
	}

	/**
	 * Variable is used for representing a Pin with a Text, Data type and value
	 * Used to represent a data variable that ends up in C# script when generating code.
	 * NewVariables are added to a Graph and then VariableNodes can be created based on those graph variables
	 * to manipulate data flow and CodeGeneration.
	 */
	struct Variable
	{
		enum class ValueType { None = 0, Flow, Bool, Int, Float, Vector2, String };

		ValueType m_ValueType;
		std::string m_Text;
		pin_type_variant m_Value;

		// For "internal use"
		uint32_t m_ID;
		// Used for tracking which nodes we need to Resize if Variable text changes
		std::vector<std::shared_ptr<Pin>> m_AttachedToPins;

		Variable()
			: m_ValueType(ValueType::Int), m_Text(GenNewVarName()), m_Value(0), m_ID(s_VarCount++), m_AttachedToPins({})
		{}

		Variable(ValueType valueType, std::string text, pin_type_variant value)
			: m_ValueType(valueType), m_Text(text), m_Value(value), m_ID(s_VarCount++), m_AttachedToPins({})
		{}
	};

	struct Pin
	{
		enum class Type { INPUT = 0, OUTPUT, FLOW_IN, FLOW_OUT };
		//enum class ValueType { None = 0, Flow, Bool, Int, Float, Vector2, String };

		Node* m_ParentNode;	// Owner of the pin
		uint32_t m_ID;	    // Pin ID

		bool m_Connected = false;
		bool m_SingleConnection = false;
		std::vector<Pin*> m_ConnectedPins;

		Type m_Type;

		std::shared_ptr<Variable> m_Variable = std::make_shared<Variable>();

		// For handling selection - global position
		ImVec2 m_Pos = { 0.0f, 0.0f };
	};

	struct NodePinData
	{
		Pin::Type m_Type;
		Variable m_Variable;
	};

	struct NodeLayout
	{
		std::vector<NodePinData> m_ParamList;

		NodeLayout(std::initializer_list<NodePinData> params)
		{
			for (auto item : params)
			{
				m_ParamList.emplace_back(item);
			}
		}

		std::vector<NodePinData>::iterator begin() { return m_ParamList.begin(); }
		std::vector<NodePinData>::iterator end() { return m_ParamList.end(); }
	};

	struct Node
	{
		enum class NodeType { None = 0, Variable, Property, Method, Statement, Event, Create };

		ImVec2 m_Pos;
		ImVec2 m_Size;
		uint32_t m_ID = 0;

		std::string m_Title;
		bool m_HasHeader = true;

		// List of Pins
		std::vector<std::shared_ptr<Pin>> m_InPins;
		std::vector<std::shared_ptr<Pin>> m_OutPins;

		NodeType m_Type = NodeType::None;
		// Used for calling code gen functions
		std::shared_ptr<CodeComponent> m_CodeComponent;

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

		// Variables that are represented with Pins, VariableNodes will be created based on variables in this list
		std::vector<std::shared_ptr<Variable>> m_Variables;

		// ImGui show tab item flag
		bool m_Show = true;

		Graph() = default;
		Graph(const Graph&) = default;
		Graph(const std::string& name) : m_Name(name) {}

		uint32_t GenerateNodeID() { return ++m_NextNodeID; }
		uint32_t GeneratePinID() { return ++m_NextPinID; }

		void RemoveVariable(Variable* var);
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

		// Lines
		float m_LineThickness;

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
	ImVec2 CalcNodeSize(Node* node);

	// Pin methods
	std::string VariableValueTypeToString(Variable::ValueType type);
	void UpdateVariableValueType(Variable* variable, Variable::ValueType type);
	float GetPinSpaceHeight();
	float GetPinSpaceWidth();
	bool IsFlowPin(Pin* pin);
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