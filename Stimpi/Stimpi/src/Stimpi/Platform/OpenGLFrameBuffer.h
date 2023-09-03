#pragma once

#include "Stimpi/Graphics/FrameBuffer.h"

namespace Stimpi
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(uint32_t width, uint32_t height);
		~OpenGLFrameBuffer();

		void BindBuffer() override;

	private:
		unsigned int m_ID;
	};
}