#include "stpch.h"
#include "Stimpi/Graphics/VertexArrayObject.h"

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLVertexArrayObject.h"

namespace Stimpi
{

	VertexArrayObject::VertexArrayObject(const VertexBufferLayout& layout)
		: m_Layout(layout)
	{

	}

	VertexArrayObject::~VertexArrayObject()
	{

	}

	VertexArrayObject* VertexArrayObject::CreateVertexArrayObject(const VertexBufferLayout& layout)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLVertexArrayObject(layout);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}

		ST_CORE_CRITICAL("Unknown GraphicsAPI");
		return nullptr;
	}

}