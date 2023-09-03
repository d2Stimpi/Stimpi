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
		uint32_t posSize;
		uint32_t colorSize;
		uint32_t texSize;
		uint32_t stride;
		uint32_t posOffset;
		uint32_t colorOffset;
		uint32_t texOffset;
		
		DataLayout(uint32_t posLen, uint32_t colorLen, uint32_t textLen) : posSize(posLen), colorSize(colorLen), texSize(textLen)
		{
			stride = (uint32_t)sizeof(float) * (posSize + colorSize + texSize);
			posOffset = 0;
			colorOffset = sizeof(float) * posSize;
			texOffset = (uint32_t)sizeof(float) * (posSize + colorSize);
		}
	};

	class ST_API VertexArrayObject
	{
	public:
		VertexArrayObject(const DataLayout& layout);
		virtual ~VertexArrayObject();

		// Called from Renderer with proper VertexArray ID
		virtual void BindArray(unsigned int id) = 0;
		// Called from Renderer
		virtual void EnableVertexAttribArray() = 0;

		// Create VAO based on Renderer API type (OpenGL)
		static VertexArrayObject* CreateVertexArrayObject(const DataLayout& layout);

	protected:
		DataLayout m_Layout;
	};
}