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
		OpenGLTexture(std::string file);
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

		bool Loaded() override;

		// For async loading
		void LoadDataAsync(std::string file) override;
	
	private:
		void CheckLoadFuture();	// to remove
		void GenerateTexture(TextureLoadData* data);	// to remove

	private:
		unsigned int m_TextureID{ 0 };

		std::future<TextureLoadData*> m_LoadFuture;	// to remove
	};
}