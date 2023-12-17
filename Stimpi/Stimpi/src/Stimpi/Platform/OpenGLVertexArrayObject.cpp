#include "stpch.h"
#include "Stimpi/Platform/OpenGLVertexArrayObject.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	static GLenum GLDataTypeFromLayoutDataType(ShaderDataType type)
	{
		switch (type)
		{
		case Stimpi::ShaderDataType::Int:		return GL_INT;
		case Stimpi::ShaderDataType::Int2:		return GL_INT;
		case Stimpi::ShaderDataType::Int3:		return GL_INT;
		case Stimpi::ShaderDataType::Int4:		return GL_INT;
		case Stimpi::ShaderDataType::Float:		return GL_FLOAT;
		case Stimpi::ShaderDataType::Float2:	return GL_FLOAT;
		case Stimpi::ShaderDataType::Float3:	return GL_FLOAT;
		case Stimpi::ShaderDataType::Float4:	return GL_FLOAT;
		}

		ST_CORE_CRITICAL("Unknown GL data type!");
		return GL_INT;
	}

	OpenGLVertexArrayObject::OpenGLVertexArrayObject(const VertexBufferLayout& layout)
		: VertexArrayObject(layout)
	{
		glGenVertexArrays(1, &m_ID);
	}

	OpenGLVertexArrayObject::~OpenGLVertexArrayObject()
	{
		glDeleteVertexArrays(1, &m_ID);
	}

	void OpenGLVertexArrayObject::BindArray()
	{
		glBindVertexArray(m_ID);
	}

	void OpenGLVertexArrayObject::Unbind()
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArrayObject::EnableVertexAttribArray()
	{
		int attributeIndex = 0;
		for (auto layout : m_Layout)
		{
			glVertexAttribPointer(attributeIndex, layout.m_Size, GLDataTypeFromLayoutDataType(layout.m_Type), GL_FALSE, m_Layout.m_Stride, (void*)layout.m_Offset);
			glEnableVertexAttribArray(attributeIndex);

			attributeIndex++;
		}
	}

}