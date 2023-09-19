#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Graphics/Texture.h"

namespace Stimpi
{
	class ST_API FrameBuffer
	{
	public:
		FrameBuffer(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}
		virtual ~FrameBuffer();

		virtual void BindBuffer() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		unsigned int GetTextureID() { return m_Texture->GetTextureID(); }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		Texture* GetTexture() { return m_Texture.get(); }

		static FrameBuffer* CreateFrameBuffer(uint32_t width, uint32_t height);
	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		std::shared_ptr<Texture> m_Texture;
	};
}