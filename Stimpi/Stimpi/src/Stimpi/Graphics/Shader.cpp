#include "stpch.h"
#include "Stimpi/Graphics/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLShader.h"
#include "Stimpi/Log.h"

#include "Stimpi/Scene/ResourceManager.h"

namespace Stimpi
{
	Shader::~Shader()
	{

	}

	Shader* Shader::CreateShader(const std::string& fileName)
	{
		// Get the project specified path where shaders are expected to be stored
		auto shaderFolderPath = std::filesystem::absolute(ResourceManager::GetResourcesPath()) / "shaders" / fileName;

		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLShader(shaderFolderPath.string());
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}

	std::shared_ptr<Stimpi::Shader> Shader::Create(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return std::make_shared<OpenGLShader>(info, vsd, fsd);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}

	void Shader::LogShaderInfo()
	{
		ST_CORE_INFO("=== Shader {} info begin ===", m_Name);
		ST_CORE_INFO("LayoutData:");
		for (auto& item : m_Info.m_ShaderLayout.m_Data)
		{
			ST_CORE_INFO("  {} {}", item.m_Name, ShaderDataTypeLength(item.m_Type));
		}
		ST_CORE_INFO("Properties: TODO", m_Name);
		ST_CORE_INFO("=== Shader {} info end ===", m_Name);
	}

	void Shader::SetUniform(const std::string name, shader_variant value)
	{
		m_UniformList.emplace(name, value);
	}

	void Shader::SetBufferedUniforms()
	{
		for (auto& uniform : m_UniformList)
		{
			auto& name = uniform.first;
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