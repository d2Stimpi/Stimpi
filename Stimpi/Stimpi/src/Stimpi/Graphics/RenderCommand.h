#pragma once

#include <initializer_list>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/VertexArrayObject.h"
#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/OrthoCamera.h"
#include "Stimpi/Graphics/FrameBuffer.h"


namespace Stimpi
{
	struct TextureData
	{
		Texture* m_Texture;
		std::vector<float> m_Data;
		uint32_t m_VertexCount;

		TextureData() : m_Texture(nullptr), m_Data({}), m_VertexCount(0) {}
		uint32_t Size() { return m_Data.size() * sizeof(float); }
		float* Data(uint32_t offsetBytes = 0) { return m_Data.data() + (offsetBytes / sizeof(float)); }
	};

	class RenderCommand
	{
	public:
		RenderCommand();
		RenderCommand(OrthoCamera* camera, Shader* shader);
		~RenderCommand();

		void PushVertexBufferData(glm::vec3 position, glm::vec3 color, glm::vec2 textureCoord);
		void PushIndexDufferData(); // TODO: not needed atm
		void UseTexture(Texture* texture);

		OrthoCamera* GetCamera() { return m_Camera; }
		Shader* GetShader() { return m_Shader; }
		std::vector<std::shared_ptr<TextureData>>& GetData() { return m_TextureDataVec; }


	private:
		std::vector<std::shared_ptr<TextureData>>::iterator TextureDataHasTexture(Texture* texture);

		OrthoCamera* m_Camera;
		Shader* m_Shader;
		std::vector<std::shared_ptr<TextureData>> m_TextureDataVec;
		std::vector<std::shared_ptr<TextureData>>::iterator m_CurrentTextureData;
	};
}