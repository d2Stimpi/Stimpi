#include "stpch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	OpenGLFrameBuffer::OpenGLFrameBuffer(FrameBufferSpecification config)
		: FrameBuffer(config)
	{
		glGenFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		m_Texture.reset(Texture::CreateFrameBufferTexture());
		m_Texture->InitEmptyTexture({ m_Spec.m_Width, m_Spec.m_Height, m_Spec.m_Channels }); // Create RGBA

		// Attach texture to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetTextureID(), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			ST_CORE_ERROR("Framebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
	}

	void OpenGLFrameBuffer::BindBuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		if ((m_Spec.m_Width == width) && (m_Spec.m_Height == height))
			return;

		m_Spec.m_Width = width;
		m_Spec.m_Height = height;
		//ST_CORE_INFO("Framebuffer size: {0}, {1}", m_Width, m_Height);

		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		m_Texture->Resize(m_Spec.m_Width, m_Spec.m_Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetTextureID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}