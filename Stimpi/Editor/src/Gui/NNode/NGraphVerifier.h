#pragma once

#include "Gui/NNode/NGraph.h"

// TODO: validate Node in/out pin count and method number of in/out params

namespace Stimpi
{
	class NGraphVerifier
	{
	public:
		static bool ValidConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest);
	};
}