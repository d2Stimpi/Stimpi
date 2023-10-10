#include "stpch.h"
#include "Stimpi/Graphics/BufferObject.h"

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLBufferObject.h"
#include "Stimpi/Log.h"

namespace Stimpi
{
	BufferObject::BufferObject(BufferObjectType type)
		: m_Type(type)
	{

	}

	BufferObject::~BufferObject()
	{

	}

	BufferObject* BufferObject::CreateBufferObject(BufferObjectType type)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLBufferObject(type);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}
}