#include "Stimpi/Graphics/RenderCommand.h"

namespace Stimpi
{
	RenderCommand::RenderCommand(Camera* camera, Shader* shader) :
		m_Camera(camera),
		m_TextureDataVec({}),
		m_CurrentTextureData({}),
		m_Shader(shader)
	{
		m_TextureDataVec.emplace_back(std::make_shared<TextureData>());
		m_CurrentTextureData = m_TextureDataVec.begin();
	}

	RenderCommand::~RenderCommand()
	{

	}

	void RenderCommand::Render()
	{
			// 1. Upload Camera data to Shader
		m_Shader->Use();
		m_Shader->SetUniform("mvp", m_Camera->GetMvpMatrix());
			// 2. BindTexture and upload to Shader
		//m_Texture->UseTexture();
		//m_Shader->SetUniform("u_texture", m_Texture->GetTextureID());
			// 3. Bind Buffer Array (VBO)
		//m_VBO->BindArray();
			// 4. Set all vertex subData
		//for(auto vdata : vdata_list)
		//	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(float) * vdata->count, vdata->data);
			// 5. Bind VBO
		//m_VBO->BindArray();
	}










	void RenderCommand::PushVertexBufferData(std::initializer_list<float> list)
	{
		for (auto elem : list)
		{
			(*m_CurrentTextureData)->m_Data.push_back(elem);
		}
		(*m_CurrentTextureData)->m_VertexCount++;
	}

	void RenderCommand::PushIndexDufferData(std::initializer_list<unsigned int> list)
	{
		for (auto elem : list)
		{
			//m_CmdIndexData.push_back(elem);
		}
		//m_IndexCount += list.size();
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
			// Texture changed, create new data obj
			m_TextureDataVec.emplace_back(std::make_shared<TextureData>());
			//m_CurrentTextureData++;
			std::advance(m_CurrentTextureData, 1);
		}
		
	}
}