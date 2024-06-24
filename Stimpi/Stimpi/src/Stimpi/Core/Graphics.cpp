#include "stpch.h"
#include "Stimpi/Graphics/Graphics.h"

namespace Stimpi
{
	std::string RenderingOrderAxisToString(const RenderingOrderAxis& axis)
	{
		switch (axis)
		{
		case RenderingOrderAxis::None:		return "None";
		case RenderingOrderAxis::X_AXIS:	return "X_AXIS";
		case RenderingOrderAxis::Y_AXIS:	return "Y_AXIS";
		case RenderingOrderAxis::Z_AXIS:	return "Z_AXIS";
		}

		return "None";
	}

	RenderingOrderAxis StringToRenderingOrderAxis(const std::string& axis)
	{
		if (axis == "None")		return RenderingOrderAxis::None;
		if (axis == "X_AXIS")		return RenderingOrderAxis::X_AXIS;
		if (axis == "Y_AXIS")		return RenderingOrderAxis::Y_AXIS;
		if (axis == "Z_AXIS")		return RenderingOrderAxis::Z_AXIS;

		return RenderingOrderAxis::None;
	}
}