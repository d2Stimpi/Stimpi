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

		void SetData(float* data, uint32_t size);
		float* GetData() { return m_Data; }

		// Called from Renderer with proper BufferObject ID
		virtual void BindBuffer(unsigned int id) = 0;
		// Called from Renderer with correct data offset
		virtual void BufferSubData(uint32_t offset) = 0;

		static BufferObject* CreateBufferObject(BufferObjectType type);
	protected:
		BufferObjectType m_Type;
		float* m_Data;
		uint32_t m_Size;
	};
}