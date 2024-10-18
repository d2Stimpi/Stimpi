#pragma once

#include "Stimpi/Graphics/VertexArrayObject.h"
#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/OrthoCamera.h"
#include "Stimpi/Graphics/FrameBuffer.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	enum class RenderCommandType { NONE = 0, QUAD, CIRLCE, LINE };

	struct VertexData
	{
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec4 m_Color = glm::vec4(0.0f);
		glm::vec2 m_TexCoord = glm::vec2(0.0f);

		VertexData(glm::vec3 position, glm::vec4 color, glm::vec2 texCoord)
			: m_Position(position), m_Color(color), m_TexCoord(texCoord) {}

		float* operator&() { return &m_Position[0]; }
	};

	struct CircleVertexData
	{
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec4 m_Color = glm::vec4(0.0f);
		glm::vec2 m_TexCoord = glm::vec2(0.0f);
		float m_Thickness = 0.0f;
		float m_Fade = 0.0f;

		CircleVertexData(glm::vec3 position, glm::vec4 color, glm::vec2 texCoord, float thickness, float fade)
			: m_Position(position), m_Color(color), m_TexCoord(texCoord), m_Thickness(thickness), m_Fade(fade) {}

		float* operator&() { return &m_Position[0]; }
	};

	struct LineVertexData
	{
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec4 m_Color = glm::vec4(0.0f);

		LineVertexData(glm::vec3 position, glm::vec4 color)
			: m_Position(position), m_Color(color) {}

		float* operator&() { return &m_Position[0]; }
	};

	struct RenderCommand
	{
		RenderCommandType m_Type;
		Shader* m_Shader;
		Texture* m_Texture;
		std::vector<VertexData> m_VertexData;
		std::vector<CircleVertexData> m_CircleVertexData;
		std::vector<LineVertexData> m_LineVertexData;
		uint32_t m_VertexCount;
		uint32_t m_VertexSize;


		RenderCommand(uint32_t vertexSize) :
			m_Type(RenderCommandType::NONE),
			m_Shader(nullptr),
			m_Texture(nullptr),
			m_VertexData({}),
			m_CircleVertexData({}),
			m_LineVertexData({}),
			m_VertexCount(0),
			m_VertexSize(vertexSize)
		{}

		uint32_t Size() { return m_VertexCount * m_VertexSize; }
		float* Data() { return &(m_VertexData[0]); }
		float* CircleData() { return &(m_CircleVertexData[0]); }
		float* LineData() { return &(m_LineVertexData[0]); }

		void PushQuadVertex(glm::vec3 position, glm::vec4 color, glm::vec2 textureCoord)
		{
			ST_CORE_ASSERT((m_Type != RenderCommandType::NONE && m_Type != RenderCommandType::QUAD));

			if (m_Type == RenderCommandType::NONE)
				m_Type = RenderCommandType::QUAD;

			m_VertexData.emplace_back(position, color, textureCoord);
			m_VertexCount++;
		}

		void PushCircleVertex(glm::vec3 position, glm::vec4 color, glm::vec2 textureCoord, float thickness, float fade)
		{
			ST_CORE_ASSERT((m_Type != RenderCommandType::NONE && m_Type != RenderCommandType::CIRLCE));

			if (m_Type == RenderCommandType::NONE)
				m_Type = RenderCommandType::CIRLCE;

			m_CircleVertexData.emplace_back(position, color, textureCoord, thickness, fade);
			m_VertexCount++;
		}

		void PushLineVertex(glm::vec3 position, glm::vec4 color)
		{
			ST_CORE_ASSERT((m_Type != RenderCommandType::NONE && m_Type != RenderCommandType::LINE));

			if (m_Type == RenderCommandType::NONE)
				m_Type = RenderCommandType::LINE;

			m_LineVertexData.emplace_back(position, color);
			m_VertexCount++;
		}

		void ClearData()
		{
			m_LineVertexData.clear();
			m_CircleVertexData.clear();
			m_VertexData.clear();
			m_VertexCount = 0;
		}
	};
}