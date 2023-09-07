#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Graphics.h"


namespace Stimpi
{
	enum class BufferObjectType { ARRAY_BUFFER = 0, ELEMENT_BUFFER };

	class ST_API BufferObject
	{
	public:
		BufferObject(BufferObjectType type);
		virtual ~BufferObject();

		// Called from Renderer with proper BufferObject ID
		virtual void InitBuffer(uint32_t capacity) = 0;
		virtual void BindBuffer() = 0;
		virtual void Unbind() = 0;
		// Called from Renderer with correct data offset
		virtual void BufferSubData(uint32_t offset, uint32_t size, float* data) = 0;

		static BufferObject* CreateBufferObject(BufferObjectType type);
	protected:
		BufferObjectType m_Type;
		uint32_t m_Capacity;
	};
}