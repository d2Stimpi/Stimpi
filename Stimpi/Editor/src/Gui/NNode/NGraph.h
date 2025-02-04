#pragma once

#include "Gui/NNode/NGraphComponents.h"

#include "Stimpi/Core/UUID.h"

namespace Stimpi
{
	struct NGraph
	{
		bool IsConnected(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest);
		void ConnectPinToPin(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest);
		void BreakPinToPinConnection(std::shared_ptr<NPinConnection> connection);
		void UpdateConnectionPoints(std::shared_ptr<NPinConnection> connection);
		std::shared_ptr<NPinConnection> FindPinToPinConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest);
		void RegenerateGraphDataAfterLoad();

		UUID m_ID;
		std::string m_Name = "Unnamed";

		// ImGui show tab item flag
		bool m_Show = true;

		std::vector<std::shared_ptr<NNode>> m_Nodes;
		std::vector<std::shared_ptr<NPinConnection>> m_PinConnections;
	};
}