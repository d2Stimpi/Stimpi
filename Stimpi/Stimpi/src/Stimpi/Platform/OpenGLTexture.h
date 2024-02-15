#pragma once

#include "Stimpi/Graphics/Texture.h"

#include <future>

namespace Stimpi
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture();
		OpenGLTexture(std::string file);
		~OpenGLTexture();

		void Delete();

		void InitEmptyTexture(uint32_t width, uint32_t height, uint32_t channems = 3) override;
		void LoadTexture(std::string file) override;
		void UseTexture() override; 
		void Bind() override;
		void Unbind() override;
		void Resize(uint32_t width, uint32_t height) override;
		unsigned int GetTextureID() override { return m_TextureID; }

		bool Loaded() override;

		// For async loading
		void LoadDataAsync(std::string file) override;
	
	private:
		void CheckLoadFuture();
		void GenerateTexture(TextureLoadData* data);

	private:
		unsigned int m_TextureID{ 0 };
		uint32_t m_NumChannels{ 0 }; // for stb_img loading

		std::future<TextureLoadData*> m_LoadFuture;
	};
}