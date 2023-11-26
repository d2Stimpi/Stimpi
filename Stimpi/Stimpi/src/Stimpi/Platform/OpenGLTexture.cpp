#include "stpch.h"
#include "Stimpi/Platform/OpenGLTexture.h"

#include <glad/glad.h>
#include <stb_image.h>
#include "Stimpi/Log.h"

#define DBG_LOG false

namespace Stimpi
{
	OpenGLTexture::OpenGLTexture()
	{

	}

	OpenGLTexture::OpenGLTexture(std::string file)
	{
		LoadTexture(file);
	}

	OpenGLTexture::~OpenGLTexture()
	{
		if(DBG_LOG) ST_CORE_INFO("Texture {0} deleted", m_TextureID);
		Delete();
	}

	void OpenGLTexture::Delete()
	{
		if (m_TextureID != 0)
		{
			glDeleteTextures(1, &m_TextureID);
		}
	}

	// Primary use for FrameBuffer texture
	void OpenGLTexture::InitEmptyTexture(uint32_t width, uint32_t height, uint32_t channels)
	{
		m_Width = width;
		m_Height = height;
		m_NumChannels = channels;

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		if (m_NumChannels == 3) // RGB
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		}
		else //RGBA
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::LoadTexture(std::string file)
	{
		// Remove old texture if one exists
		Delete();

		stbi_set_flip_vertically_on_load(true); // TODO: Move somewhere on init
		unsigned char* data = stbi_load(file.c_str(), (int*)&m_Width, (int*)&m_Height, (int*)&m_NumChannels, 0);
		if (DBG_LOG) ST_CORE_INFO("Texture Loaded: {0}, numChannels: {1}", file.c_str(), m_NumChannels);
		if (data)
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (m_NumChannels == 3) // RGB
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else //RGBA
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			}
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
		if (m_TextureID != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}
		else
		{
			ST_CORE_ERROR("Binding invalid texture!");
		}
	}

	void OpenGLTexture::Bind()
	{
		if (m_TextureID != 0)	// TODO: fix this so it cant occur, move init to constructor
		{
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}
		else
		{
			ST_CORE_ERROR("Binding invalid texture!");
		}
	}

	void OpenGLTexture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		if (m_NumChannels == 3) // RGB
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		}
		else //RGBA
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		}
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}