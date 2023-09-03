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

		void BindBuffer(unsigned int id) override;
		void BufferSubData(uint32_t offset) override;

	private:
	};
}