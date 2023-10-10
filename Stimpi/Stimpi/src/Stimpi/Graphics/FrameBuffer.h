#pragma once

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Graphics/Texture.h"

namespace Stimpi
{
	struct FrameBufferConfig
	{
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;

		FrameBufferConfig(uint32_t width, uint32_t height, uint32_t channels)
			: m_Width(width), m_Height(height), m_Channels(channels)
		{}
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(FrameBufferConfig config) : m_Width(config.m_Width), m_Height(config.m_Height), m_Channels(config.m_Channels) {}
		virtual ~FrameBuffer();

		virtual void BindBuffer() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		unsigned int GetTextureID() { return m_Texture->GetTextureID(); }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }
		Texture* GetTexture() { return m_Texture.get(); }

		static FrameBuffer* CreateFrameBuffer(FrameBufferConfig config);
	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
		std::shared_ptr<Texture> m_Texture;
	};
}