#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	enum class GraphicsAPI { None = 0, OpenGL };

	class Graphics
	{
	public:
		static GraphicsAPI GetAPI() { return GraphicsAPI::OpenGL; };
	};

	/* Comon types */
	
	enum class ST_API DrawType { STATIC_DRAW = 0, STREAM_DRAW };
}