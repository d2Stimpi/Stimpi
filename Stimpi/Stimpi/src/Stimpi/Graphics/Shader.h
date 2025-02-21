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
	using shader_variant = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

	using VertexShaderData = std::string;
	using FragmentShaderData = std::string;

	enum class ShaderDataType
	{
		Int = 0, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
		Unknown
	};

	// For simplicity, recognized (expected) glsl types only
	static ShaderDataType StringToShaderType(const std::string& type)
	{
		if (type == "int")			return ShaderDataType::Int;
		else if (type == "ivec2")	return ShaderDataType::Int2;
		else if (type == "ivec3")	return ShaderDataType::Int3;
		else if (type == "ivec4")	return ShaderDataType::Int4;
		else if (type == "float")	return ShaderDataType::Float;
		else if (type == "vec2")	return ShaderDataType::Float2;
		else if (type == "vec3")	return ShaderDataType::Float3;
		else if (type == "vec4")	return ShaderDataType::Float4;
		
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
		}

		ST_CORE_CRITICAL("Unknown Shader data type!");
		return 0;
	}

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
		unsigned int m_VAOHandle = 0;  // TODO: when shader handle asset is used move this outside of struct

		ShaderInfo() = default;
		ShaderInfo(const std::string& name, ShaderLayout& layout)
			: m_Name(name), m_ShaderLayout(layout), m_VAOHandle(0) {}
	};

	/* Storage of shader data for extra layout values, per entity that uses the shader
	 * Assumption:
	 *	first 3 layers of custom shader have to be Pos/Color/TexCoord 3/3/2
	 *  only support float data type
	 */
	struct ST_API ShaderData
	{
		std::vector<float> m_Data;

		void ResizeData(ShaderInfo info)
		{
			size_t dataSize = 0;
			for (auto& layout : info.m_ShaderLayout)
			{
				dataSize += layout.m_Size;
			}
			// reduce by 8 floats for Pos/Color/TexCoord
			dataSize -= 8;
			m_Data.resize(dataSize);
		}
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

		void SetUniform(const std::string name, shader_variant value);
		void SetBufferedUniforms();
		void ClearBufferedUniforms();

		std::string& GetName() { return m_Name; }
		ShaderInfo& GetInfo() { return m_Info; }

		static std::shared_ptr<Shader> Create(ShaderInfo info, VertexShaderData vsd, FragmentShaderData fsd);

		static AssetType GetTypeStatic() { return AssetType::SHADER; }
		AssetType GetType() override { return GetTypeStatic(); }

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

		// name : value for buffering Uniforms
		std::unordered_map<std::string, shader_variant> m_UniformList;
	};
}