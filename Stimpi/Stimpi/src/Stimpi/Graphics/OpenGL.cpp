#include "OpenGL.h"

#include <glad/glad.h>

#include "Stimpi/Log.h"

namespace Stimpi
{
	GLContext::~GLContext()
	{
		SDL_GL_DeleteContext(s_GLcontext);
	}

	void GLContext::CreateContext(Window* window)
	{
		s_GLcontext = SDL_GL_CreateContext(GetSDLWindow(window));
		SDL_GL_MakeCurrent(GetSDLWindow(window), s_GLcontext);
		SDL_GL_SetSwapInterval(1); // Enable vsync
	}

	/*************************************************************************************************/
	/******************************** Generic OpenGL Calls *******************************************/
	/*************************************************************************************************/

	void OpenGLRenderer::DrawElements(uint32_t size)
	{
		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);
	}
}