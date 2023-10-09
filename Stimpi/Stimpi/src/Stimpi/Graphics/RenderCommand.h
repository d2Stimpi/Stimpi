#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/VertexArrayObject.h"
#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/OrthoCamera.h"
#include "Stimpi/Graphics/FrameBuffer.h"

namespace Stimpi
{
	struct VertexData
	{
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_Color = glm::vec3(0.0f);
		glm::vec2 m_TexCoord = glm::vec2(0.0f);

		float* operator&() { return &m_Position[0]; }
	};

	struct RenderCommand
	{
		OrthoCamera* m_Camera; // use in Renderer2D to map cmds by camera
		Shader* m_Shader;
		Texture* m_Texture;
		std::vector<VertexData> m_VertexData;
		uint32_t m_VertexCount;
		uint32_t m_VertexSize;

		RenderCommand(OrthoCamera* camera, uint32_t vertexSize)
			: m_Camera(camera), m_Shader(nullptr), m_Texture(nullptr), m_VertexData({}), m_VertexCount(0), m_VertexSize(vertexSize)
		{}

		uint32_t Size() { return m_VertexCount * m_VertexSize; }
		float* Data() { return &(m_VertexData[0]); }

		void PushVertex(glm::vec3 position, glm::vec3 color, glm::vec2 textureCoord)
		{
			VertexData vertex;
			
			vertex.m_Position = position;
			vertex.m_Color = color;
			vertex.m_TexCoord = textureCoord;

			m_VertexData.push_back(vertex);
			m_VertexCount++;
		}
	};
}