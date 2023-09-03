#include "Stimpi/Platform/OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	OpenGLTexture::OpenGLTexture(std::string file)
	{
		LoadTexture(file);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		Delete();
	}

	void OpenGLTexture::Delete()
	{
		if (mTextureID != 0)
		{
			glDeleteTextures(1, &mTextureID);
		}
	}

	void OpenGLTexture::LoadTexture(std::string file)
	{
		// Remove old dexture if one exists
		Delete();

		stbi_set_flip_vertically_on_load(true); // TODO: Move somwhere on init
		unsigned char* data = stbi_load(file.c_str(), &mWidth, &mHeight, &mNumChannels, 0);
		if (data)
		{
			glGenTextures(1, &mTextureID);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			//glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
		else
		{
			ST_CORE_ERROR("Failed to load texture: {0}", file);
		}
		stbi_image_free(data);
	}

	void OpenGLTexture::UseTexture()
	{
		if (mTextureID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mTextureID);
		}
		else
		{
			ST_CORE_ERROR("Binding invalid texture!");
		}
	}
}