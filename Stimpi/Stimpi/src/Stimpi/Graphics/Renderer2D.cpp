#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	Renderer2D::Renderer2D()
	{
		m_RenderAPI = RenderAPI::CreateRenderAPI();
		// TODO: get global window size? what size is FB?
		m_FrameBuffer.reset(FrameBuffer::CreateFrameBuffer(1920, 1080));

		//Init VAO, VBO - TODO: handling more VAOs 4k/per
		m_VAO.reset(VertexArrayObject::CreateVertexArrayObject({ 3, 0, 2 }));
		m_VAO->BindArray();

		m_VBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_VBO->BindBuffer();
		m_VBO->InitBuffer(VERTEX_ARRAY_SIZE);

		m_VAO->EnableVertexAttribArray();
	}

	Renderer2D::~Renderer2D()
	{
		delete m_RenderAPI;
		m_RenderAPI = nullptr;
	}

	Renderer2D* Renderer2D::Instace()
	{
		static auto m_Instace = std::make_unique<Renderer2D>();
		return m_Instace.get();
	}

	//To link with ImGui
	FrameBuffer* Renderer2D::GetFrameBuffer()
	{
		return m_FrameBuffer.get();
	}

	void Renderer2D::BeginScene(Camera* camera, Shader* shader)
	{
		m_ActiveCmd = std::make_shared<RenderCommand>(camera, shader);
	}

	void Renderer2D::EndScene()
	{

	}

	// TODO: hande u,v coords
	void Renderer2D::PushQuad(float x, float y, float width, float height, float u, float v)
	{
		auto left = x;
		auto right = x + width;
		auto bottom = y;
		auto top = y + height;

		// Push Quad vertex data in Layout format { 3, 0, 2 }
		m_ActiveCmd->PushVertexBufferData({ left, bottom, 0.0f, 0.0f, 0.0f });
		m_ActiveCmd->PushVertexBufferData({ right, top, 0.0f, 1.0f, 1.0f });
		m_ActiveCmd->PushVertexBufferData({ left, top, 0.0f, 0.0f, 1.0f });

		m_ActiveCmd->PushVertexBufferData({ left, bottom, 0.0f, 0.0f, 0.0f });
		m_ActiveCmd->PushVertexBufferData({ right, bottom, 0.0f, 1.0f, 0.0f });
		m_ActiveCmd->PushVertexBufferData({ right, top, 0.0f, 1.0f, 1.0f });

		//TODO: Push Quad index data
	}

	void Renderer2D::UseTexture(Texture* texture)
	{
		m_ActiveCmd->UseTexture(texture);
	}

	/* Render logic here below */

	void Renderer2D::StartFrame()
	{
		// Select FBO to render to
		m_FrameBuffer->BindBuffer();
		m_RenderAPI->Clear(0.5f, 0.5f, 0.5f, 1.0f);	 // TODO: make as param / configurable
	}

	void Renderer2D::DrawFrame()
	{
		auto shader = m_ActiveCmd->GetShader();
		auto camera = m_ActiveCmd->GetCamera();

		shader->Use();
		shader->SetUniform("mvp", camera->GetMvpMatrix());
		shader->SetUniform("u_texture", 0);

		m_VAO->BindArray();
		m_VBO->BindBuffer();

		auto cmds = m_ActiveCmd->GetData();
		for (auto cmd : cmds)
		{
			if (cmd->m_Texture != nullptr)
			{
				cmd->m_Texture->UseTexture();
			}

			m_VBO->BufferSubData(0, cmd->m_Data.size() * sizeof(float), cmd->m_Data.data());
			m_RenderAPI->DrawArrays(DrawElementsMode::TRIANGLES, 0, cmd->m_VertexCount);
		}

	}

	void Renderer2D::EndFrame()
	{
		// Unbind stuff
		m_FrameBuffer->Unbind();
		m_RenderAPI->UnbindTexture();
	}
}