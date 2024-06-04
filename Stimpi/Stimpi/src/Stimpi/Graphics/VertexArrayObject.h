#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Log.h"

/* 
*  Usage:
* 
*  vbo(data, layout)
*  vbo.Bind()
* 
* layout - pos? color? tex? ( 3 3 2 - default)
* 
* VAO - constructed from VBO (vertex data) and EBO (indices)
*/

namespace Stimpi
{
	enum class ShaderDataType
	{
		Int = 0, Int2, Int3, Int4,
		Float, Float2, Float3, Float4,
	};

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

	struct ST_API VertexBufferLayout
	{
		std::vector<LayoutData> m_Layout;
		uint32_t m_Stride = 0;

		VertexBufferLayout() {} // Temp default ctor
		VertexBufferLayout(std::initializer_list<LayoutData> list)
		{
			m_Stride = 0;
			for (auto item : list)
			{
				item.m_Offset = m_Stride;
				m_Stride += ShaderDataTypeSize(item.m_Type);
				m_Layout.push_back(item);
			}
		}

		std::vector<LayoutData>::iterator begin() { return m_Layout.begin(); }
		std::vector<LayoutData>::iterator end() { return m_Layout.end(); }
	};

	class ST_API VertexArrayObject
	{
	public:
		VertexArrayObject(const VertexBufferLayout& layout);
		virtual ~VertexArrayObject();

		// Called from Renderer with proper VertexArray ID
		virtual void BindArray() = 0;
		virtual void Unbind() = 0;
		// Called from Renderer
		virtual void EnableVertexAttribArray() = 0;

		uint32_t VertexSize() { return m_Layout.m_Stride; }

		// Create VAO based on Renderer API type (OpenGL)
		static VertexArrayObject* CreateVertexArrayObject(const VertexBufferLayout& layout);

	protected:
		VertexBufferLayout m_Layout;
	};
}