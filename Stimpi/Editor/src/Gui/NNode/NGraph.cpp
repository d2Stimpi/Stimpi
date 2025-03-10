#include "stpch.h"
#include "Gui/NNode/NGraph.h"

#include "Gui/NNode/NGraphVerifier.h"
#include "Gui/NNode/NGraphStyle.h"

namespace Stimpi
{

	bool NGraph::IsConnected(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest)
	{
		if (!src->m_Connected || !dest->m_Connected)
			return false;

		for (auto& connected : src->m_ConnectedPins)
		{
			if ((connected->m_ParentNode->m_ID == dest->m_ParentNode->m_ID)	// check id parent node matches
				&& (connected->m_ID == dest->m_ID))
			{
				return true;
			}
		}

		return false;
	}

	void NGraph::ConnectPinToPin(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest)
	{
		// Check if src is out pin and if dest is input
		if ((src->m_Type == NPin::Type::Out && dest->m_Type == NPin::Type::In) ||
			(src->m_Type == NPin::Type::In && dest->m_Type == NPin::Type::Out))
		{
			// If input pin has active connection do nothing - make the user break it first
			if ((!src->m_Connected || !src->m_SingleConnection) && !dest->m_Connected)
			{
				// Check type compatibility
				if (!NGraphVerifier::ValidConnection(src, dest))
					return;

				if (IsConnected(src, dest) == false)
				{
					src->m_Connected = true;
					dest->m_Connected = true;

					src->m_ConnectedPins.push_back(dest);
					dest->m_ConnectedPins.push_back(src);

					// Global pin to pin connections
					std::shared_ptr<NPinConnection> newConnection = std::make_shared<NPinConnection>(src, dest);
					newConnection->CalculateBezierPoints(s_PanelStyle.m_ConnectionSegments);
					m_PinConnections.push_back(newConnection);
				}
			}
		}
	}

	void NGraph::BreakPinToPinConnection(std::shared_ptr<NPinConnection> connection)
	{
		if (!connection)
			return;

		auto& p1 = connection->m_Src;
		auto& p2 = connection->m_Dest;

		p1->m_ConnectedPins.erase(std::find(p1->m_ConnectedPins.begin(), p1->m_ConnectedPins.end(), p2));
		if (p1->m_ConnectedPins.empty())
			p1->m_Connected = false;

		p2->m_ConnectedPins.erase(std::find(p2->m_ConnectedPins.begin(), p2->m_ConnectedPins.end(), p1));
		if (p2->m_ConnectedPins.empty())
			p2->m_Connected = false;

		// Remove PinConnection
		m_PinConnections.erase(std::find(m_PinConnections.begin(), m_PinConnections.end(), connection));
	}

	void NGraph::UpdateConnectionPoints(std::shared_ptr<NPinConnection> connection)
	{
		if (connection)
		{
			connection->m_BezierLinePoints.clear();
			connection->CalculateBezierPoints(s_PanelStyle.m_ConnectionSegments);
		}
	}

	std::shared_ptr<NPinConnection> NGraph::FindPinToPinConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest)
	{
		if (src == nullptr || dest == nullptr)
			return nullptr;

		for (auto& connection : m_PinConnections)
		{
			if (connection->m_Src->m_ID == src->m_ID && connection->m_Dest->m_ID == dest->m_ID)
				return connection;

			if (connection->m_Src->m_ID == dest->m_ID && connection->m_Dest->m_ID == src->m_ID)
				return connection;
		}

		return nullptr;
	}

	void NGraph::RegenerateGraphDataAfterLoad()
	{
		for (auto& connection : m_PinConnections)
		{
			UpdateConnectionPoints(connection);
		}
	}

	std::string NGraph::GetLabel()
	{
		return fmt::format("{}##{}", m_Name, m_ID);
	}

}