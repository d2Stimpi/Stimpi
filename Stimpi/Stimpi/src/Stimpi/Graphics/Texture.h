#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class ST_API Texture
	{
	public:
		virtual ~Texture();

		virtual void InitEmptyTexture(uint32_t width, uint32_t height, uint32_t channems = 3) = 0;
		virtual void LoadTexture(std::string file) = 0;
		virtual void UseTexture() = 0;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual unsigned int GetTextureID() = 0;

		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		static Texture* CreateTexture(std::string file);
		static Texture* CreateFrameBufferTexture();

		// AssetManager
		static Texture* Create(std::string file) { return CreateTexture(file); }
		virtual bool Loaded() = 0;
	protected:
		uint32_t m_Width{ 0 };
		uint32_t m_Height{ 0 };
	};
}