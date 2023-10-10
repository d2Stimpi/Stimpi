#include "stpch.h"
#include "Stimpi/Graphics/Shader.h"

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

	void Shader::SetUniform(const std::string name, shader_variant value)
	{
		m_UniformList.emplace(name, value);
	}

	void Shader::SetBufferedUniforms()
	{
		for (auto uniform : m_UniformList)
		{
			auto name = uniform.first;
			auto value = uniform.second;
			std::visit([&, name](auto&& arg) {
				SetUniformImpl(name, arg);
				}, value);
		}
	}

	void Shader::ClearBufferedUniforms()
	{
		m_UniformList.clear();
	}
}