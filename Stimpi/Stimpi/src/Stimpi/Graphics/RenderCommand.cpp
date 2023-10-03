#include "Stimpi/Graphics/RenderCommand.h"

namespace Stimpi
{
	RenderCommand::RenderCommand()
		: RenderCommand(nullptr, nullptr)
	{

	}

	RenderCommand::RenderCommand(Camera* camera, Shader* shader) :
		m_Camera(camera),
		m_Shader(shader),
		m_TextureDataVec({}),
		m_CurrentTextureData({})
	{
		m_TextureDataVec.emplace_back(std::make_shared<TextureData>());
		m_CurrentTextureData = m_TextureDataVec.begin();
	}

	RenderCommand::~RenderCommand()
	{

	}

	void RenderCommand::PushVertexBufferData(glm::vec3 position, glm::vec3 color, glm::vec2 textureCoord)
	{
		(*m_CurrentTextureData)->m_Data.push_back(position.x);
		(*m_CurrentTextureData)->m_Data.push_back(position.y);
		(*m_CurrentTextureData)->m_Data.push_back(position.z);
		(*m_CurrentTextureData)->m_Data.push_back(color.r);
		(*m_CurrentTextureData)->m_Data.push_back(color.g);
		(*m_CurrentTextureData)->m_Data.push_back(color.b);
		(*m_CurrentTextureData)->m_Data.push_back(textureCoord.x);
		(*m_CurrentTextureData)->m_Data.push_back(textureCoord.y);
		(*m_CurrentTextureData)->m_VertexCount++;
	}

	void RenderCommand::PushIndexDufferData()
	{
		//TODO: when rdy to suport indices (ElementArray)
	}

	void RenderCommand::UseTexture(Texture* texture)
	{
		// If no Texture set in active data; set texture, otherwise create new data obj
		auto data = *m_CurrentTextureData;
		if ((data->m_Texture == nullptr) || (data->m_Texture == texture))
		{
			data->m_Texture = texture;
		}
		else
		{
			// Is Data with the texture already present
			auto foundTextureDataIter = TextureDataHasTexture(texture);
			if (foundTextureDataIter != std::end(m_TextureDataVec))
			{
				// Texture already used before, use the same data struct element
				m_CurrentTextureData = foundTextureDataIter;
			}
			else
			{
				// Texture changed, create new data obj
				m_TextureDataVec.emplace_back(std::make_shared<TextureData>());
				m_CurrentTextureData = m_TextureDataVec.end() - 1;
				(*m_CurrentTextureData)->m_Texture = texture;
			}
		}
		
	}

	std::vector<std::shared_ptr<TextureData>>::iterator RenderCommand::TextureDataHasTexture(Texture* texture)
	{
		auto cmpFunc = [texture](std::shared_ptr<TextureData> elem) -> bool
		{
			return elem->m_Texture->GetTextureID() == texture->GetTextureID();
		};

		return std::find_if(std::begin(m_TextureDataVec), std::end(m_TextureDataVec), cmpFunc);
	}
}