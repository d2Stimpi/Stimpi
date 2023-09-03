#include "Stimpi/Graphics/Shader.h"

#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLShader.h"
#include "Stimpi/Log.h"

namespace Stimpi
{
	Shader::~Shader()
	{

	}

	Shader* Shader::CreateShader(const std::string& fileName)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLShader(fileName);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}
}