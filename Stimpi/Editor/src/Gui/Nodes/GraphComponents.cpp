#include "stpch.h"
#include "Gui/Nodes/GraphComponents.h"

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

#pragma endregion Node


#pragma region Pin
	/**
	 * Helper Pin static functions
	 */

	float GetPinSpaceHeight()
	{
		return s_Style.m_PinRadius * 2.0f;
	}

	float GetPinSpaceWidth()
	{
		return s_Style.m_PinRadius * 2.0f + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
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

	// TODO: consider moving to graph class
	void ConnectPinToPin(Pin* src, Pin* dest, Graph* graph)
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
}