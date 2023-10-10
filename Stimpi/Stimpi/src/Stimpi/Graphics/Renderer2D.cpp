#include "stpch.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	Renderer2D::Renderer2D()
	{
		m_RenderAPI = RenderAPI::CreateRenderAPI();
		// TODO: get global window size? what size is FB?
		m_FrameBuffer.reset(FrameBuffer::CreateFrameBuffer({ 1280, 720, 4 }));

		//Init VAO, VBO - TODO: handling more VAOs 4k/per
		m_VAO.reset(VertexArrayObject::CreateVertexArrayObject({ 3, 3, 2 }));
		m_VAO->BindArray();

		m_VBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_VBO->BindBuffer();
		m_VBO->InitBuffer(VERTEX_ARRAY_SIZE);

		m_VAO->EnableVertexAttribArray();

		// Debug data
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;

		// Init cmd storage
		m_NewRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_ActiveCamera, m_VAO->VertexSize()));
		m_ActiveNewRenderCmdIter = std::end(m_NewRenderCmds) - 1;
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

	// New
	void Renderer2D::BeginScene(OrthoCamera* camera)
	{
		auto currnetCmd = *m_ActiveNewRenderCmdIter;

		m_ActiveCamera = camera;
		currnetCmd->m_Camera = m_ActiveCamera;
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}
	
	void Renderer2D::Submit(glm::vec4 quad, Texture* texture, Shader* shader)
	{	
		CheckTextureBatching(texture);
		auto currnetCmd = *m_ActiveNewRenderCmdIter;

		CheckCapacity();
		// First time call Submit after BeginScene
		if ((currnetCmd->m_Texture == nullptr) && ((currnetCmd->m_Shader == nullptr)))
		{
			PushQuadVertexData(currnetCmd.get(), quad);
			currnetCmd->m_Texture = texture;
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		} 
		else if ((currnetCmd->m_Texture != texture) || (currnetCmd->m_Shader != shader))
		{
			// If shader or texture changed
			Flush(); 
			currnetCmd = *m_ActiveNewRenderCmdIter;
			PushQuadVertexData(currnetCmd.get(), quad);
			currnetCmd->m_Texture = texture;
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		}
		else
		{
			// Batching vertex data
			PushQuadVertexData(currnetCmd.get(), quad);
		}
	}

	void Renderer2D::Submit(glm::vec4 quad, Shader* shader)
	{
		Submit(quad, nullptr, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, glm::vec3 color, Shader* shader)
	{
		auto currnetCmd = *m_ActiveNewRenderCmdIter;

		// Flush if we have a texture set from other Submits or shader changed
		if ((currnetCmd->m_Texture != nullptr) || (currnetCmd->m_Shader != shader))
		{
			Flush();
			currnetCmd = *m_ActiveNewRenderCmdIter;
		}

		PushQuadVertexData(currnetCmd.get(), quad, color);
		currnetCmd->m_Shader = shader;
	}

	void Renderer2D::Flush()
	{
		auto currnetCmd = *m_ActiveNewRenderCmdIter;

		// For now set ViewProj camera uniform here
		currnetCmd->m_Shader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_NewRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_ActiveCamera, m_VAO->VertexSize()));
		m_ActiveNewRenderCmdIter = std::end(m_NewRenderCmds) - 1;
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
		m_FrameBuffer->BindBuffer();
		m_RenderAPI->SetViewport(0, 0, m_FrameBuffer->GetWidth(), m_FrameBuffer->GetHeight());
		m_RenderAPI->Clear(0.5f, 0.5f, 0.5f, 1.0f);	 // TODO: make as param / configurable
		m_RenderAPI->EnableBlend();
	}

	void Renderer2D::DrawFrame()
	{
		for (auto renderCmd : m_NewRenderCmds)
		{
			// Skip last as it won't be filled with data
			if (renderCmd != *m_ActiveNewRenderCmdIter)
				DrawRenderCmd(renderCmd);
		}
		m_RenderedCmdCnt = m_NewRenderCmds.size() - 1; // last won't be filled with data
	}

	void Renderer2D::EndFrame()
	{
		// TODO: support multiple render targets here (other FBO)
		// Unbind stuff
		m_FrameBuffer->Unbind();
		m_RenderAPI->UnbindTexture();

		// Clear Render commands
		m_NewRenderCmds.clear();
		m_NewRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_ActiveCamera, m_VAO->VertexSize()));
		m_ActiveNewRenderCmdIter = std::end(m_NewRenderCmds) - 1;

		// Clear Debug data
		ShowDebugData();
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;
	}

	void Renderer2D::DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto camera = renderCmd->m_Camera;
		auto shader = renderCmd->m_Shader;

		shader->Use();
		shader->SetBufferedUniforms();

		m_VAO->BindArray();
		m_VBO->BindBuffer();

		if (renderCmd->m_Texture != nullptr)
			renderCmd->m_Texture->UseTexture();

		m_VBO->BufferSubData(0, renderCmd->Size(), renderCmd->Data());
		m_RenderAPI->DrawArrays(DrawElementsMode::TRIANGLES, 0, renderCmd->m_VertexCount);
		m_DrawCallCnt++;

		shader->ClearBufferedUniforms();
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

	void Renderer2D::CheckCapacity()
	{
		auto currnetCmd = *m_ActiveNewRenderCmdIter;

		if (currnetCmd->m_VertexCount >= VERTEX_CMD_CAPACITY)
		{
			Flush();
		}
	}

	void Renderer2D::CheckTextureBatching(Texture* texture)
	{
		auto found = std::find_if(std::begin(m_NewRenderCmds), std::end(m_NewRenderCmds), [&texture](auto elem) -> bool {
			if (texture != nullptr && elem->m_Texture != nullptr)
				return texture->GetTextureID() == elem->m_Texture->GetTextureID();
			else
				return false;
			});

		if (found != std::end(m_NewRenderCmds))
		{
			m_ActiveNewRenderCmdIter = found;
		}
	}

	void Renderer2D::PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec3 color /*= { 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{
		auto left = quad.x;
		auto right = quad.x + quad.z;
		auto bottom = quad.y;
		auto top = quad.y + quad.w;
		
		cmd->PushVertex({ left, bottom, 0.0f }, color, { min.x, min.y });
		cmd->PushVertex({ right, top, 0.0f }, color, { max.x, max.y });
		cmd->PushVertex({ left, top, 0.0f }, color, { min.x, max.y });
		
		cmd->PushVertex({ left, bottom, 0.0f }, color, { min.x, min.y });
		cmd->PushVertex({ right, bottom, 0.0f }, color, { max.x, min.y });
		cmd->PushVertex({ right, top, 0.0f }, color, { max.x, max.y });
	}

}