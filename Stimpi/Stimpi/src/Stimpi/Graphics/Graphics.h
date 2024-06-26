#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	enum class GraphicsAPI { None = 0, OpenGL };

	enum class RenderingOrderAxis { None = 0, X_AXIS, Y_AXIS, Z_AXIS };

	std::string RenderingOrderAxisToString(const RenderingOrderAxis& axis);
	RenderingOrderAxis StringToRenderingOrderAxis(const std::string& axis);

	/**
	 * Used for general Rendering configurations
	 */
	struct GraphicsConfig
	{
		RenderingOrderAxis m_RenderingOrderAxis = RenderingOrderAxis::None;
	};

	/**
	 * Used for Sorting objects in layers when submitting for rendering
	 */
	struct SortingLayer
	{
		std::string m_Name;

		SortingLayer() = default;
		SortingLayer(const SortingLayer&) = default;
		SortingLayer(std::string name)
			: m_Name(name)
		{}
	};

	class Graphics
	{
	public:
		static GraphicsAPI GetAPI() { return GraphicsAPI::OpenGL; };
	};

	/* Common types */
	
	enum class ST_API DrawType { STATIC_DRAW = 0, STREAM_DRAW };
}