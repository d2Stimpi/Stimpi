#include "stpch.h"
#include "OpenGLBufferObject.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	GLenum OpenGLTargetBuffer(BufferObjectType type)
	{
		switch (type)
		{
		case BufferObjectType::ARRAY_BUFFER: return GL_ARRAY_BUFFER;
		case BufferObjectType::ELEMENT_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
		default:
			ST_CORE_ERROR("BufferObject: unknown buffer taget type! (default GL_ARRAY_BUFFER)");
			return GL_ARRAY_BUFFER;
		}
	}

	OpenGLBufferObject::OpenGLBufferObject(BufferObjectType type)
		: BufferObject(type)
	{
		glGenBuffers(1, &m_ID);
	}

	OpenGLBufferObject::~OpenGLBufferObject()
	{
		glDeleteBuffers(1, &m_ID);
	}

	void OpenGLBufferObject::InitBuffer(uint32_t capacity)
	{
		glBindBuffer(OpenGLTargetBuffer(m_Type), m_ID);
		glBufferData(OpenGLTargetBuffer(m_Type), sizeof(float) * capacity, NULL, GL_DYNAMIC_DRAW); // TODO: make params customizable
	}


	void OpenGLBufferObject::BindBuffer()
	{
		glBindBuffer(OpenGLTargetBuffer(m_Type), m_ID);
	}

	void OpenGLBufferObject::Unbind()
	{
		glBindBuffer(OpenGLTargetBuffer(m_Type), 0);
	}

	void OpenGLBufferObject::BufferSubData(uint32_t offset, uint32_t size, float* data)
	{
		glBufferSubData(OpenGLTargetBuffer(m_Type), offset, size, data);
	}
}