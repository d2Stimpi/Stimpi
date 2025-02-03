#pragma once

#include "Gui/NNode/NGraphComponents.h"

namespace Stimpi
{

	class NNodeRegistry
	{
	public:
		static void InitializeNodeRegisrty();

		static std::vector<std::string> GetNodeNamesList();
		static std::shared_ptr<NNode> CreateNodeByName(const std::string& name);
	};

}