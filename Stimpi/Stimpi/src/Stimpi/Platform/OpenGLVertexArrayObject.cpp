#include "Stimpi/Platform/OpenGLVertexArrayObject.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	OpenGLVertexArrayObject::OpenGLVertexArrayObject(const DataLayout& layout)
		: VertexArrayObject(layout)
	{

	}

	OpenGLVertexArrayObject::~OpenGLVertexArrayObject()
	{

	}

	void OpenGLVertexArrayObject::BindArray(unsigned int id)
	{
		glBindVertexArray(id);
	}

	void OpenGLVertexArrayObject::EnableVertexAttribArray()
	{
		// position attribute
		if (m_Layout.posSize > 0)
		{
			glVertexAttribPointer(0, m_Layout.posSize, GL_FLOAT, GL_FALSE, m_Layout.stride, (void*)m_Layout.posOffset);
			glEnableVertexAttribArray(0);
		}
		// color attribute
		if (m_Layout.colorSize > 0)
		{
			glVertexAttribPointer(1, m_Layout.colorSize, GL_FLOAT, GL_FALSE, m_Layout.stride, (void*)m_Layout.colorOffset);
			glEnableVertexAttribArray(1);
		}
		// texture coord attribute
		if (m_Layout.texSize > 0)
		{
			glVertexAttribPointer(2, m_Layout.texSize, GL_FLOAT, GL_FALSE, m_Layout.stride, (void*)m_Layout.texOffset);
			glEnableVertexAttribArray(2);
		}
	}
}