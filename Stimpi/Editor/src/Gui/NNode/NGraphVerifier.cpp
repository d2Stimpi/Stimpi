#include "stpch.h"
#include "Gui/NNode/NGraphVerifier.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	bool NGraphVerifier::ValidConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest)
	{
		bool valid = src->m_DataType == dest->m_DataType;

		// TODO: report error to UI
		if (!valid)
			ST_WARN("Pins have incompatible data types!");

		return valid;
	}

}