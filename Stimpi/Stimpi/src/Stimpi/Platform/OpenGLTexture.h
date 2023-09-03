#pragma once

#include "Stimpi/Graphics/Texture.h"

namespace Stimpi
{
	class OpenGLTexture : public Texture
	{
	public:
		OpenGLTexture() {};
		OpenGLTexture(std::string file);
		~OpenGLTexture();

		void Delete();
		void LoadTexture(std::string file) override;
		void UseTexture() override;
	private:
		unsigned int mTextureID{ 0 };
		int mNumChannels{ 0 }; // for stb_img loading
	};
}