#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	enum class GraphicsAPI { None = 0, OpenGL };

	enum class RenderingOrderAxis { None = 0, X_AXIS, Y_AXIS, Z_AXIS };

	std::string RenderingOrderAxisToString(const RenderingOrderAxis& axis);
	RenderingOrderAxis StringToRenderingOrderAxis(const std::string& axis);

	struct GraphicsConfig
	{
		RenderingOrderAxis m_RenderingOrderAxis = RenderingOrderAxis::None;
	};

	class Graphics
	{
	public:
		static GraphicsAPI GetAPI() { return GraphicsAPI::OpenGL; };
	};

	/* Common types */
	
	enum class ST_API DrawType { STATIC_DRAW = 0, STREAM_DRAW };
}