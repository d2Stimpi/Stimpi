#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"

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

	struct ST_API DataLayout
	{
		uint32_t m_PosSize;
		uint32_t m_ColorSize;
		uint32_t m_TexSize;
		uint32_t m_Stride;
		uint32_t m_PosOffset;
		uint32_t m_ColorOffset;
		uint32_t m_TexOffset;
		
		DataLayout(uint32_t posLen, uint32_t colorLen, uint32_t textLen) : m_PosSize(posLen), m_ColorSize(colorLen), m_TexSize(textLen)
		{
			m_Stride = (uint32_t)sizeof(float) * (m_PosSize + m_ColorSize + m_TexSize);
			m_PosOffset = 0;
			m_ColorOffset = sizeof(float) * m_PosSize;
			m_TexOffset = (uint32_t)sizeof(float) * (m_PosSize + m_ColorSize);
		}
	};

	class ST_API VertexArrayObject
	{
	public:
		VertexArrayObject(const DataLayout& layout);
		virtual ~VertexArrayObject();

		// Called from Renderer with proper VertexArray ID
		virtual void BindArray() = 0;
		virtual void Unbind() = 0;
		// Called from Renderer
		virtual void EnableVertexAttribArray() = 0;

		uint32_t VertexSize() { return m_Layout.m_Stride; }

		// Create VAO based on Renderer API type (OpenGL)
		static VertexArrayObject* CreateVertexArrayObject(const DataLayout& layout);

	protected:
		DataLayout m_Layout;
	};
}