#pragma once

#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/Graphics.h"

namespace Stimpi
{
	class OpenGLBufferObject : public BufferObject
	{
	public:
		OpenGLBufferObject(BufferObjectType type);
		~OpenGLBufferObject();

		void InitBuffer(uint32_t capacity) override;
		void BindBuffer() override;
		void Unbind() override;
		void BufferSubData(uint32_t offset, uint32_t size, float* data) override;

	private:
		unsigned int m_ID;
	};
}