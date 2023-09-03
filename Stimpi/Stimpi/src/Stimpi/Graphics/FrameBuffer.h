#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"

namespace Stimpi
{
	class ST_API FrameBuffer
	{
	public:
		FrameBuffer(uint32_t width, uint32_t height) : m_TextureID(0), m_Width(width), m_Height(height) {}
		virtual ~FrameBuffer();

		virtual void BindBuffer() = 0;

		unsigned int GetTextureID() { return m_TextureID; }

		static FrameBuffer* CreateFrameBuffer(uint32_t width, uint32_t height);
	protected:
		unsigned int m_TextureID;
		uint32_t m_Width;
		uint32_t m_Height;
	};
}