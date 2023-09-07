#include "Stimpi/Platform/OpenGLVertexArrayObject.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	OpenGLVertexArrayObject::OpenGLVertexArrayObject(const DataLayout& layout)
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
		// position attribute
		if (m_Layout.m_PosSize > 0)
		{
			glVertexAttribPointer(0, m_Layout.m_PosSize, GL_FLOAT, GL_FALSE, m_Layout.m_Stride, (void*)m_Layout.m_PosOffset);
			glEnableVertexAttribArray(0);
		}
		// color attribute
		if (m_Layout.m_ColorSize > 0)
		{
			glVertexAttribPointer(1, m_Layout.m_ColorSize, GL_FLOAT, GL_FALSE, m_Layout.m_Stride, (void*)m_Layout.m_ColorOffset);
			glEnableVertexAttribArray(1);
		}
		// texture coord attribute
		if (m_Layout.m_TexSize > 0)
		{
			glVertexAttribPointer(2, m_Layout.m_TexSize, GL_FLOAT, GL_FALSE, m_Layout.m_Stride, (void*)m_Layout.m_TexOffset);
			glEnableVertexAttribArray(2);
		}
	}
}