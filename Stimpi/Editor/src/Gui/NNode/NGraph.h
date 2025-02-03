#pragma once

#include "Gui/NNode/NGraphComponents.h"

#include "Stimpi/Core/UUID.h"

namespace Stimpi
{
	struct NGraph
	{
		UUID m_ID;
		std::string m_Name = "Unnamed";

		// ImGui show tab item flag
		bool m_Show = true;

		std::vector<std::shared_ptr<NNode>> m_Nodes;
	};
}