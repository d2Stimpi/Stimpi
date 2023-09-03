#pragma once

#include "Stimpi/Graphics/VertexArrayObject.h"

namespace Stimpi
{
	class OpenGLVertexArrayObject : public VertexArrayObject
	{
	public:
		OpenGLVertexArrayObject(const DataLayout& layout);
		~OpenGLVertexArrayObject();

		void BindArray(unsigned int id) override;
		void EnableVertexAttribArray() override;

	private:
	};
}