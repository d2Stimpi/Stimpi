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

	}

	OpenGLBufferObject::~OpenGLBufferObject()
	{

	}

	void OpenGLBufferObject::BindBuffer(unsigned int id)
	{
		glBindBuffer(OpenGLTargetBuffer(m_Type), id);
	}

	void OpenGLBufferObject::BufferSubData(uint32_t offset)
	{
		glBufferSubData(OpenGLTargetBuffer(m_Type), offset, m_Size, m_Data);
	}
}