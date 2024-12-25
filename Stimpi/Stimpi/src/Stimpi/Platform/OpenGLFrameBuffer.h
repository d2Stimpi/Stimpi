#pragma once

#include "Stimpi/Graphics/FrameBuffer.h"

namespace Stimpi
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(FrameBufferSpecification config);
		~OpenGLFrameBuffer();

		void BindBuffer() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height) override;

	private:
		unsigned int m_ID;
	};
}