#include "stpch.h"
#include "Stimpi/Graphics/Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLShader.h"
#include "Stimpi/Log.h"

#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/VisualScripting/ExecTree.h"

namespace Stimpi
{
	Shader::~Shader()
	{

	}

	std::shared_ptr<Shader> Shader::Create(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return std::make_shared<OpenGLShader>(info, vsd, fsd);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}

	void Shader::SetCustomExecTree(std::shared_ptr<ExecTree> execTree)
	{
		m_ExecTree = execTree;
	}

	std::shared_ptr<ExecTree>& Shader::GetCustomExecTree()
	{
		return m_ExecTree;
	}

	void Shader::LogShaderInfo()
	{
		ST_CORE_INFO("=== Shader {} info begin ===", m_Name);
		ST_CORE_INFO("LayoutData:");
		for (auto& item : m_Info.m_ShaderLayout.m_Data)
		{
			ST_CORE_INFO(" layout - {} {}", item.m_Name, ShaderDataTypeLength(item.m_Type));
		}
		ST_CORE_INFO("Properties: TODO", m_Name);

		ST_CORE_INFO("Unifroms:");
		for (auto& uni : m_Info.m_Uniforms)
		{
			ST_CORE_INFO(" uniform - {} {}", uni.m_Name, ShaderDataTypeLength(uni.m_Type));
		}
		ST_CORE_INFO("=== Shader {} info end ===\n", m_Name);
	}

	void Shader::SetLayerData(const std::string& name, shader_variant value)
	{
		m_LayerDataList[name] = value;
	}

	shader_variant Shader::GetLayerData(const std::string& name)
	{
		auto found = m_LayerDataList.find(name);
		if (found != m_LayerDataList.end())
			return found->second;

		return 0;
	}

	void Shader::ClearLayerData()
	{
		m_LayerDataList.clear();
	}

	void Shader::SetUniform(const std::string& name, shader_variant value)
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