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

	/* VertexBuffer to hold continuous array of vertex data */
	class VertexBuffer
	{
	public:
		VertexBuffer(uint32_t capacity, uint32_t vertexSize) : m_Data({}), m_Capacity(capacity), m_VertexSize(vertexSize) {}

		float* GetRawData() { return m_Data.data(); }
		size_t GetCount() { return m_Data.size() / m_VertexSize; }

		std::vector<float>& GetData() { return m_Data; }

	private:
		std::vector<float> m_Data;
		size_t m_Capacity;
		size_t m_VertexSize;
	};
}