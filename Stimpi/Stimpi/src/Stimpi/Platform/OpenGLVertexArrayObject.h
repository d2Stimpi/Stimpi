#pragma once

#include "Stimpi/Graphics/VertexArrayObject.h"

namespace Stimpi
{
	class OpenGLVertexArrayObject : public VertexArrayObject
	{
	public:
		OpenGLVertexArrayObject(const VertexBufferLayout& layout);
		~OpenGLVertexArrayObject();

		unsigned int GetID() override;
		void BindArray() override;
		void Unbind() override;
		void EnableVertexAttribArray() override;

	private:
		unsigned int m_ID;
	};
}