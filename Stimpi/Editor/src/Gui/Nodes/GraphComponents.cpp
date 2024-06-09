#include "stpch.h"
#include "Gui/Nodes/GraphComponents.h"

#include "Gui/Nodes/GraphPanel.h"

namespace Stimpi
{
	NodePanelStyle s_Style;

	/**
	 * Helper Utility functions
	 */
#pragma region Utility

	float PointDistance(ImVec2 p1, ImVec2 p2)
	{
		return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	}

	void RegenerateGraphDataAfterLoad(Graph* graph)
	{
		for (auto connection : graph->m_PinConnections)
		{
			UpdateConnectionPoints(connection.get());
		}
	}

#pragma endregion Utility

	/**
	 * Helper Node static functions
	 */
#pragma region Node

	// Return Node's space (without header).
	ImVec2 GetAvailableNodeSpaceSize(Node* node)
	{
		ImVec2 available = node->m_Size;

		available.y -= s_Style.m_HeaderHeight;

		return available;
	}

	// Return Node's extra size. Including header size and any other style added sizes.
	ImVec2 GetNodeExtraSize(Node* node)
	{
		ImVec2 space = GetAvailableNodeSpaceSize(node);

		return { node->m_Size.x - space.x, node->m_Size.y - space.y };
	}

	// Check if Node's space can hold certain size.
	bool IsNodeSpaceSizeAvailable(Node* node, ImVec2 size)
	{
		ImVec2 available = GetAvailableNodeSpaceSize(node);

		return (available.x >= size.x) && (available.y >= size.y);
	}

	// Resize the Node's space lacking axis size, account for header sizes, etc.
	// Return
	//   true - if any axis was resized
	bool ResizeNodeSpace(Node* node, ImVec2 size)
	{
		bool resized = false;
		ImVec2 newSize = size;
		ImVec2 extra = GetNodeExtraSize(node);

		newSize.x += extra.x;
		newSize.y += extra.y;

		if (node->m_Size.x < newSize.x)
		{
			node->m_Size.x = newSize.x;
			resized = true;
		}

		if (node->m_Size.y < newSize.y)
		{
			node->m_Size.y = newSize.y;
			resized = true;
		}

		return true;
	}

	ImVec2 CalcNodeSize(Node* node)
	{
		ImVec2 size = { 0.0f, 0.0f};

		// Pin space
		int pinCount = std::max(node->m_InPins.size(), node->m_OutPins.size());
		size.y += pinCount * (s_Style.m_PinRadius + s_Style.m_PinSpacing) + s_Style.m_PinOffset;

		// Width
		if (!node->m_InPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : node->m_InPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Variable->m_Text.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth + s_Style.m_PinOffset + s_Style.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!node->m_OutPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : node->m_OutPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Variable->m_Text.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth + s_Style.m_PinOffset + s_Style.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!node->m_OutPins.empty() && !node->m_InPins.empty())
		{
			size.x += s_Style.m_PinSpacing*2;
		}

		if (node->m_HasHeader)
		{
			size.y += s_Style.m_HeaderHeight + 10;

			float width = 2 * s_Style.m_HeaderTextOffset.x;
			width += ImGui::CalcTextSize(node->m_Title.c_str()).x;

			if (width > size.x)
			{
				size.x = width;
			}
		}
		else
		{
			size.x += s_Style.m_HeaderTextOffset.x;
		}

		return size;
	}

#pragma endregion Node


#pragma region Pin
	/**
	 * Helper Pin static functions
	 */

	std::string VariableValueTypeToString(Variable::ValueType type)
	{
		switch (type)
		{
		case Variable::ValueType::None: return "";
		case Variable::ValueType::Flow: return "";
		case Variable::ValueType::Bool: return "Bool";
		case Variable::ValueType::Int: return "Int";
		case Variable::ValueType::Float: return "Float";
		case Variable::ValueType::Vector2: return "Vector2";
		case Variable::ValueType::String: return "String";
		}

		return "";
	}

	void UpdateVariableValueType(Variable* variable, Variable::ValueType type)
	{
		variable->m_ValueType = type;

		switch (type)
		{
		case Variable::ValueType::None: variable->m_Value = (int)0; break;  	 // should never get here
		case Variable::ValueType::Flow: variable->m_Value = (int)0; break;	 // should never get here
		case Variable::ValueType::Bool: variable->m_Value = true; break;
		case Variable::ValueType::Int: variable->m_Value = (int)0; break;
		case Variable::ValueType::Float: variable->m_Value = 0.0f; break;
		case Variable::ValueType::Vector2: variable->m_Value = glm::vec2(0.0f, 0.0f); break;
		case Variable::ValueType::String: variable->m_Value = std::string(); break;
		}
	}

	float GetPinSpaceHeight()
	{
		return s_Style.m_PinRadius * 2.0f;
	}

	float GetPinSpaceWidth()
	{
		return s_Style.m_PinRadius * 2.0f + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
	}

	bool IsFlowPin(Pin* pin)
	{
		if (pin->m_Type == Pin::Type::FLOW_IN || pin->m_Type == Pin::Type::FLOW_OUT)
			return true;

		return false;
	}

	bool IsConnected(Pin* src, Pin* dest)
	{
		if (!src->m_Connected || !dest->m_Connected)
			return false;

		for (auto connected : src->m_ConnectedPins)
		{
			if ((connected->m_ParentNode->m_ID == dest->m_ParentNode->m_ID)	// check id parent node matches
				&& (connected->m_ID == dest->m_ID))
			{
				return true;
			}
		}

		return false;
	}

	void ConnectPinToPin(Pin* src, Pin* dest, Graph* graph)
	{
		// Check if src is out pin and if dest is input
		if ((src->m_Type == Pin::Type::OUTPUT && dest->m_Type == Pin::Type::INPUT) ||
			(src->m_Type == Pin::Type::FLOW_OUT && dest->m_Type == Pin::Type::FLOW_IN) ||
			(src->m_Type == Pin::Type::FLOW_IN && dest->m_Type == Pin::Type::FLOW_OUT) ||
			(src->m_Type == Pin::Type::INPUT && dest->m_Type == Pin::Type::OUTPUT))
		{
			// If input pin has active connection do nothing - make the user break it first
			if ((dest->m_ConnectedPins.empty() && !IsFlowPin(dest)) ||
				(IsFlowPin(src) && !src->m_Connected && !dest->m_Connected) ||
				(IsFlowPin(dest) && !src->m_Connected && !dest->m_Connected))
			{
				// Check type compatibility
				if (dest->m_Variable->m_ValueType == src->m_Variable->m_ValueType)
				{
					if (IsConnected(src, dest) == false)
					{
						src->m_Connected = true;
						dest->m_Connected = true;

						src->m_ConnectedPins.emplace_back(dest);
						dest->m_ConnectedPins.emplace_back(src);

						// Global pin to pin connections
						std::shared_ptr<PinConnection> newConnection = std::make_shared<PinConnection>(src, dest);
						CalculateBezierPoints(newConnection.get(), src, dest, s_Style.m_ConnectionSegments);
						graph->m_PinConnections.emplace_back(newConnection);
					}
				}
				else
				{
					// TODO: maybe auto add type converter
				}
			}
		}
	}

	void BreakPinToPinConnection(PinConnection* connection, Graph* graph)
	{
		if (!connection)
			return;

		// Delete p1 and p2 mutual reference
		Pin* p1 = connection->m_SourcePin;
		Pin* p2 = connection->m_DestinationPin;

		p1->m_ConnectedPins.erase(std::remove(p1->m_ConnectedPins.begin(), p1->m_ConnectedPins.end(), p2));
		if (p1->m_ConnectedPins.empty())
			p1->m_Connected = false;

		p2->m_ConnectedPins.erase(std::remove(p2->m_ConnectedPins.begin(), p2->m_ConnectedPins.end(), p1));
		if (p2->m_ConnectedPins.empty())
			p2->m_Connected = false;

		// Remove from vector
		for (auto it = graph->m_PinConnections.begin(); it != graph->m_PinConnections.end();)
		{
			auto itConn = (*it).get();
			if (itConn == connection)
			{
				graph->m_PinConnections.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	void UpdateConnectionPoints(PinConnection* connection)
	{
		if (connection)
		{
			connection->m_BezierLinePoints.clear();
			CalculateBezierPoints(connection, connection->m_SourcePin, connection->m_DestinationPin, s_Style.m_ConnectionSegments);
		}
	}

	PinConnection* FindPinToPinConnection(Pin* src, Pin* dest, Graph* graph)
	{
		if (src == nullptr || dest == nullptr)
			return nullptr;

		for (auto& connection : graph->m_PinConnections)
		{
			if (connection->m_SourcePin == src && connection->m_DestinationPin == dest)
				return connection.get();

			if (connection->m_SourcePin == dest && connection->m_DestinationPin == src)
				return connection.get();
		}

		return nullptr;
	}

#pragma endregion Pin

	/**
	 *  Helper Connection static functions
	 */
#pragma region Connection

	ImVec2 CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, start.y };
	}

	ImVec2 CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, end.y };
	}

	/* Based on ImGui's ImBezierCubicCalc */
	ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
	{
		float u = 1.0f - t;
		float w1 = u * u * u;
		float w2 = 3 * u * u * t;
		float w3 = 3 * u * t * t;
		float w4 = t * t * t;
		return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
	}

	void CalculateBezierPoints(PinConnection* connection, Pin* src, Pin* dest, uint32_t segments)
	{
		ImVec2 startPoint = src->m_Pos;
		ImVec2 endPoint = dest->m_Pos;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);

		connection->m_BezierLinePoints.push_back(startPoint);

		float t_step = 1.0f / (float)segments;
		for (int i_step = 1; i_step <= segments; i_step++)
			connection->m_BezierLinePoints.push_back(BezierCubicCalc(startPoint, middlePoint1, middlePoint2, endPoint, t_step * i_step));
	}

#pragma endregion Connection

	void Graph::RemoveVariable(Variable* var)
	{
		m_Variables.erase(std::find_if(m_Variables.begin(), m_Variables.end(), [&var](auto rem)
			{
				return var->m_ID == rem->m_ID;
			}));
	}


	Stimpi::Node* Graph::FindNodeByName(std::string name)
	{
		auto found = std::find_if(m_Nodes.begin(), m_Nodes.end(), [&name](auto node)
			{
				return node->m_Title == name;
			});

		if (found != m_Nodes.end())
		{
			return (*found).get();
		}
		else
		{
			return nullptr;
		}
	}

	Pin::Pin(Node* parent, Type type, std::shared_ptr<Variable> var)
		: m_ParentNode(parent), m_Type(type), m_Variable(var), m_ID(GraphPanel::GetGlobalActiveGraph()->GeneratePinID())
	{
	}

}