#pragma once

#include "Stimpi/Graphics/Texture.h"

#include <future>

namespace Stimpi
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture();
		OpenGLTexture(TextureSpecification spec);
		~OpenGLTexture();

		void Delete();

		void InitEmptyTexture(TextureSpecification spec) override;
		void LoadTexture(std::string file) override;
		void SetData(unsigned char* data) override;
		void UseTexture() override; 
		void Bind() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height) override;
		unsigned int GetTextureID() override { return m_TextureID; }

	private:
		unsigned int m_TextureID{ 0 };
	};
}