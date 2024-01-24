#include "stpch.h"
#include "Stimpi/Platform/OpenGLRenderAPI.h"

#include <glad/glad.h>
#include "Stimpi/Log.h"

namespace Stimpi
{
	GLenum OpenGLDrawMode(DrawElementsMode mode)
	{
		switch (mode)
		{
		case DrawElementsMode::TRIANGLES: return GL_TRIANGLES;
		case DrawElementsMode::LINES: return GL_LINES;
		default:
			ST_CORE_ERROR("OpenGLDrawMode: unknown draw mode! (default GL_TRIANGLES)");
			return GL_TRIANGLES;
		}
	}

	OpenGLRenderAPI::~OpenGLRenderAPI()
	{

	}

	void OpenGLRenderAPI::EnableBlend()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void OpenGLRenderAPI::Clear(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void OpenGLRenderAPI::UnbindTexture()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRenderAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRenderAPI::GenBuffers(uint32_t num, unsigned int* buffers)
	{
		glGenBuffers(num, buffers);
	}

	void OpenGLRenderAPI::GenVertexArrays(uint32_t num, unsigned int* buffers)
	{
		glGenVertexArrays(num, buffers);
	}

	void OpenGLRenderAPI::DrawElements(DrawElementsMode mode, uint32_t count, uint32_t offset)
	{
		glDrawElements(OpenGLDrawMode(mode), count, GL_UNSIGNED_INT, (char*)nullptr + offset);
	}

	void OpenGLRenderAPI::DrawArrays(DrawElementsMode mode, uint32_t first, uint32_t count)
	{
		glDrawArrays(OpenGLDrawMode(mode), first, count);
	}

	void OpenGLRenderAPI::SetLineWidth(float width)
	{
		glLineWidth(width);
	}
}