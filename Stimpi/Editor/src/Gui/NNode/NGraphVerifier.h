#pragma once

#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	class NGraphVerifier
	{
	public:
		static bool ValidConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest);
	};
}