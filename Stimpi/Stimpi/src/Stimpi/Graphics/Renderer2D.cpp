#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	Renderer2D::Renderer2D()
	{
		m_RenderAPI = RenderAPI::CreateRenderAPI();
		// TODO: get global window size? what size is FB?
		m_FrameBuffer.reset(FrameBuffer::CreateFrameBuffer(1280, 720));

		//Init VAO, VBO - TODO: handling more VAOs 4k/per
		m_VAO.reset(VertexArrayObject::CreateVertexArrayObject({ 3, 3, 2 }));
		m_VAO->BindArray();

		m_VBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_VBO->BindBuffer();
		m_VBO->InitBuffer(VERTEX_ARRAY_SIZE);

		m_VAO->EnableVertexAttribArray();

		// Dbg data
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;
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
		// TODO: support multiple render targets here (other FBO)
		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(camera, shader));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;
	}

	void Renderer2D::EndScene()
	{
		//TODO: combine same Shader/Camera data
		//TODO: support multiple render targets here (other FBO)
	}

	// TODO: handle UVs
	void Renderer2D::PushQuad(glm::vec4 quad, glm::vec3 color, glm::vec2 min, glm::vec2 max)
	{
		auto left = quad.x;
		auto right = quad.x + quad.z;
		auto bottom = quad.y;
		auto top = quad.y + quad.w;

		auto activeCmd = *m_ActiveRenderCmdIter;

		// Push Quad vertex data in Layout format { 3, 3, 2 }
		activeCmd->PushVertexBufferData({ left, bottom, 0.0f }, color, { min.x, min.y });
		activeCmd->PushVertexBufferData({ right, top, 0.0f }, color, { max.x, max.y });
		activeCmd->PushVertexBufferData({ left, top, 0.0f }, color, { min.x, max.y });

		activeCmd->PushVertexBufferData({ left, bottom, 0.0f }, color, { min.x, min.y });
		activeCmd->PushVertexBufferData({ right, bottom, 0.0f }, color, { max.x, min.y });
		activeCmd->PushVertexBufferData({ right, top, 0.0f }, color, { max.x, max.y });

		//TODO: Support Vertex indexing (ElementArray)
	}

	void Renderer2D::Submit(glm::vec4 quad)
	{
		PushQuad(quad);
	}

	void Renderer2D::Submit(glm::vec4 quad, Texture* texture)
	{
		UseTexture(texture);
		PushQuad(quad);
	}

	void Renderer2D::Submit(glm::vec4 quad, glm::vec3 color)
	{
		(*m_ActiveRenderCmdIter)->GetShader()->SetUniform("u_useColor", 1);
		UseTexture(nullptr);
		PushQuad(quad, color);
	}

	void Renderer2D::UseTexture(Texture* texture)
	{
		(*m_ActiveRenderCmdIter)->UseTexture(texture);
	}

	void Renderer2D::RenderTarget(FrameBuffer* target)
	{
		//TODO: if needed
	}

	void Renderer2D::ResizeCanvas(uint32_t width, uint32_t height)
	{
		m_FrameBuffer->Resize(width, height);
	}

	/* Render logic here below */

	void Renderer2D::StartFrame()
	{
		// TODO: support multiple render targets here (other FBO)
		// Select FBO to render to
		m_FrameBuffer->BindBuffer();
		m_RenderAPI->SetViewport(0, 0, m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight());
		m_RenderAPI->Clear(0.5f, 0.5f, 0.5f, 1.0f);	 // TODO: make as param / configurable
	}

	void Renderer2D::DrawFrame()
	{
		for (auto renderCmd : m_RenderCmds)
		{
			DrawRenderCmd(renderCmd);
		}
		m_RenderedCmdCnt = m_RenderCmds.size();
	}

	void Renderer2D::DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto shader = renderCmd->GetShader();
		auto camera = renderCmd->GetCamera();

		shader->Use();
		shader->SetBufferedUniforms();

		m_VAO->BindArray();
		m_VBO->BindBuffer();

		auto cmds = renderCmd->GetData();
		for (auto cmd : cmds)
		{

			if (cmd->m_Texture != nullptr)
			{
				cmd->m_Texture->UseTexture();
			}

			// Not so clean and happy way to handle VBO size - TODO: consider rework
			// Check if VBO max size is surpassed - TODO: more testing, with something like simple particle emitter
			if (cmd->Size() >= VERTEX_ARRAY_SIZE)
			{
				// Cycle trough all vertex data
				uint32_t vertexRendered = 0;
				uint32_t vertexRemaining = cmd->m_VertexCount;
				uint32_t vertexSize = m_VAO->VertexSize();
				uint32_t vertexCapacity = (uint32_t)(VERTEX_ARRAY_SIZE / vertexSize);
				uint32_t vertexToDraw = vertexCapacity;

				while (vertexRendered < cmd->m_VertexCount)
				{
					m_VBO->BufferSubData(0, vertexToDraw * vertexSize, cmd->Data(vertexRendered * vertexSize));
					m_RenderAPI->DrawArrays(DrawElementsMode::TRIANGLES, 0, vertexToDraw);
					m_DrawCallCnt++;

					vertexRendered += vertexToDraw;

					vertexRemaining -= vertexToDraw;
					if (vertexRemaining < vertexCapacity)
						vertexToDraw = vertexRemaining;
				}

			}
			else
			{
				m_VBO->BufferSubData(0, cmd->Size(), cmd->Data());
				m_RenderAPI->DrawArrays(DrawElementsMode::TRIANGLES, 0, cmd->m_VertexCount);
				m_DrawCallCnt++;
			}
		}

		shader->ClearBufferedUniforms();
	}

	void Renderer2D::EndFrame()
	{
		// TODO: support multiple render targets here (other FBO)
		// Unbind stuff
		m_FrameBuffer->Unbind();
		m_RenderAPI->UnbindTexture();

		// Clear Render Cmds
		m_RenderCmds.clear();

		// Clear Dbg data
		ShowDebugData();
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;
	}

	void Renderer2D::ShowDebugData()
	{
		if (RENDERER_DBG)
		{
			// Log only if changed from last frame
			static uint32_t loggedDrawCallCnt = 0;
			static uint32_t loggedRenderCmdCnt = 0;
			if ((loggedDrawCallCnt != m_DrawCallCnt) || (loggedRenderCmdCnt != m_RenderedCmdCnt))
			{
				ST_CORE_INFO("Renderer2D - Total per frame - Draw calls: {0}", m_DrawCallCnt);
				ST_CORE_INFO("Renderer2D - Total per frame - Rendered Cmds: {0}", m_RenderedCmdCnt);
			}
			loggedDrawCallCnt = m_DrawCallCnt;
			loggedRenderCmdCnt = m_RenderedCmdCnt;
		}
	}
}