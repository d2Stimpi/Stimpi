#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Shader.h"
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
	struct ST_API VertexBufferLayout
	{
		std::vector<LayoutData> m_Layout;
		uint32_t m_Stride = 0;

		VertexBufferLayout() = delete; // Temp default ctor
		VertexBufferLayout(std::initializer_list<LayoutData> list)
		{
			BuildLayoutData(list);
		}

		VertexBufferLayout(std::vector<LayoutData> list)
		{
			BuildLayoutData(list);
		}

		std::vector<LayoutData>::iterator begin() { return m_Layout.begin(); }
		std::vector<LayoutData>::iterator end() { return m_Layout.end(); }

	private:
		void BuildLayoutData(std::vector<LayoutData> list)
		{
			m_Stride = 0;
			for (auto item : list)
			{
				item.m_Offset = m_Stride;
				m_Stride += ShaderDataTypeSize(item.m_Type);
				m_Layout.push_back(item);
			}
		}
	};

	class ST_API VertexArrayObject
	{
	public:
		VertexArrayObject(const VertexBufferLayout& layout);
		virtual ~VertexArrayObject();

		virtual unsigned int GetID() = 0;
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