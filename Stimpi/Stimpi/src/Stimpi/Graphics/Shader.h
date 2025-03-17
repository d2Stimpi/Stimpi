#pragma once

#include "stpch.h"
#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/Asset.h"

#include <type_traits>
#include <unordered_map>

#include <glm/glm.hpp>

/*
* Shader shader(fileName):
* shader.Use();
* shader.SetUniform(name, value)
*  - name - string
*  - value - int, float, bool, glm::matX, glm::vecX 
*/

// TODO: decouple Shader and VBO files, move the data types to new shared header file

namespace Stimpi
{
	class ExecTree;

	using shader_variant = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;
	using UniformType = shader_variant;

	using VertexShaderData = std::string;
	using FragmentShaderData = std::string;

	enum class ShaderDataType
	{
		// Scalars and Vectors
		Int = 0, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Mat4,
		// Opaque types
		Sampler2D,
		Unknown
	};

	// For simplicity, recognized (expected) glsl types only
	static ShaderDataType StringToShaderType(const std::string& type)
	{
		if (type == "int")				return ShaderDataType::Int;
		else if (type == "ivec2")		return ShaderDataType::Int2;
		else if (type == "ivec3")		return ShaderDataType::Int3;
		else if (type == "ivec4")		return ShaderDataType::Int4;
		else if (type == "float")		return ShaderDataType::Float;
		else if (type == "vec2")		return ShaderDataType::Float2;
		else if (type == "vec3")		return ShaderDataType::Float3;
		else if (type == "vec4")		return ShaderDataType::Float4;
		else if (type == "mat4")		return ShaderDataType::Mat4;
		else if (type == "sampler2D")	return ShaderDataType::Sampler2D;
		
		ST_CORE_CRITICAL("Unknown Shader layout type: {}", type);
		return ShaderDataType::Unknown;
	}

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case Stimpi::ShaderDataType::Int:		return sizeof(uint32_t);
		case Stimpi::ShaderDataType::Int2:		return sizeof(uint32_t) * 2;
		case Stimpi::ShaderDataType::Int3:		return sizeof(uint32_t) * 3;
		case Stimpi::ShaderDataType::Int4:		return sizeof(uint32_t) * 4;
		case Stimpi::ShaderDataType::Float:		return sizeof(float);
		case Stimpi::ShaderDataType::Float2:	return sizeof(float) * 2;
		case Stimpi::ShaderDataType::Float3:	return sizeof(float) * 3;
		case Stimpi::ShaderDataType::Float4:	return sizeof(float) * 4;
		case Stimpi::ShaderDataType::Mat4:		return sizeof(float) * 4 * 4;
		case Stimpi::ShaderDataType::Sampler2D: return sizeof(uint32_t);
		}

		ST_CORE_CRITICAL("Unknown Shader data type!");
		return 0;
	}

	static uint32_t ShaderDataTypeLength(ShaderDataType type)
	{
		switch (type)
		{
		case Stimpi::ShaderDataType::Int:		return 1;
		case Stimpi::ShaderDataType::Int2:		return 2;
		case Stimpi::ShaderDataType::Int3:		return 3;
		case Stimpi::ShaderDataType::Int4:		return 4;
		case Stimpi::ShaderDataType::Float:		return 1;
		case Stimpi::ShaderDataType::Float2:	return 2;
		case Stimpi::ShaderDataType::Float3:	return 3;
		case Stimpi::ShaderDataType::Float4:	return 4;
		case Stimpi::ShaderDataType::Mat4:		return 16;
		case Stimpi::ShaderDataType::Sampler2D: return 1;
		}

		ST_CORE_CRITICAL("Unknown Shader data type!");
		return 0;
	}

	struct ST_API Uniform
	{
		ShaderDataType m_Type = ShaderDataType::Unknown;
		std::string m_Name = "";

		Uniform() = default;
		Uniform(const Uniform&) = default;
		Uniform(ShaderDataType type, const std::string & name)
			: m_Type(type), m_Name(name) {}
	};

	struct ST_API LayoutData
	{
		ShaderDataType m_Type;
		std::string m_Name;

		uint32_t m_Offset;
		uint32_t m_Size;

		LayoutData(ShaderDataType type, const std::string& name)
			: m_Type(type), m_Name(name)
		{
			m_Size = ShaderDataTypeLength(type);
			m_Offset = 0;
		}
	};

	struct ST_API ShaderLayout
	{
		std::vector<LayoutData> m_Data;

		ShaderLayout() = default;
		ShaderLayout(std::initializer_list<LayoutData> list)
		{
			for (auto item : list)
			{
				m_Data.push_back(item);
			}
		}

		std::vector<LayoutData>::iterator begin() { return m_Data.begin(); }
		std::vector<LayoutData>::iterator end() { return m_Data.end(); }
	};

	struct ST_API ShaderInfo
	{
		std::string m_Name;
		ShaderLayout m_ShaderLayout;
		// TODO: parse and store properties (uniforms)
		std::vector<Uniform> m_Uniforms;
		unsigned int m_VAOHandle = 0;  // TODO: when shader handle asset is used move this outside of struct

		ShaderInfo() = default;
		ShaderInfo(const std::string& name, ShaderLayout& layout)
			: m_Name(name), m_ShaderLayout(layout), m_VAOHandle(0) {}
	};

	class ST_API Shader : public Asset
	{
	public:
		Shader(const std::string& fileName) : m_Name(fileName) {}
		Shader(ShaderInfo info) : m_Name(info.m_Name), m_Info(info) {}
		virtual ~Shader();

		virtual unsigned int GetShaderID() = 0;

		// Also pass all "buffered" uniform data to shader program
		virtual void Use() = 0;
		virtual bool Loaded() = 0;

		void SetUniform(const std::string& name, shader_variant value);
		void SetBufferedUniforms();
		void ClearBufferedUniforms();

		void SetLayerData(const std::string& name, shader_variant value);
		shader_variant GetLayerData(const std::string& name);
		void ClearLayerData();

		std::string& GetName() { return m_Name; }
		ShaderInfo& GetInfo() { return m_Info; }

		static std::shared_ptr<Shader> Create(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd);

		static AssetType GetTypeStatic() { return AssetType::SHADER; }
		AssetType GetType() override { return GetTypeStatic(); }

		// Custom shader support
		void SetCustomExecTree(std::shared_ptr<ExecTree> execTree);
		std::shared_ptr<ExecTree>& GetCustomExecTree();

		// Debug
		void LogShaderInfo();

	private:
		virtual void SetUniformImpl(const std::string& name, int value) = 0;
		virtual void SetUniformImpl(const std::string& name, float value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec2 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec3 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::vec4 value) = 0;
		virtual void SetUniformImpl(const std::string& name, glm::mat4 value) = 0;

		virtual void CheckForErrors(unsigned int shader, std::string type) = 0;

		std::string m_Name;
		ShaderInfo m_Info;

		//TODO: layer data map just like uniform list, but it will be handled differently in Renderer
		std::unordered_map<std::string, shader_variant> m_LayerDataList;

		// name : value for buffering Uniforms
		std::unordered_map<std::string, shader_variant> m_UniformList;

		// For custom shader support
		std::shared_ptr<ExecTree> m_ExecTree;
	};
}