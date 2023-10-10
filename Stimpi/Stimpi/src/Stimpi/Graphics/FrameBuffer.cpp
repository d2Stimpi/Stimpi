#include "stpch.h"
#include "Stimpi/Graphics/FrameBuffer.h"

#include "Stimpi/Log.h"
#include "Stimpi/Platform/OpenGLFrameBuffer.h"

namespace Stimpi
{
	FrameBuffer::~FrameBuffer()
	{

	}

	FrameBuffer* FrameBuffer::CreateFrameBuffer(FrameBufferConfig config)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLFrameBuffer(config);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}

		ST_CORE_CRITICAL("Unknown GraphicsAPI");
		return nullptr;
	}
}