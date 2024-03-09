#include "stpch.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Log.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Stimpi
{
	// Quad vertex data for rendering by Transforms
	glm::vec4 s_QuadVertexPosition[4] = { {-0.5f, -0.5f, 0.0f, 1.0f},
										  { 0.5f, -0.5f, 0.0f, 1.0f},
										  { 0.5f,  0.5f, 0.0f, 1.0f},
										  {-0.5f,  0.5f, 0.0f, 1.0f} };

	Renderer2D::Renderer2D()
	{
		m_RenderAPI = RenderAPI::CreateRenderAPI();
		// TODO: get global window size? what size is FB?
		m_FrameBuffer.reset(FrameBuffer::CreateFrameBuffer({ 1280, 720, 4 }));

		// Init Quad rendering VAO, VBO
		m_VAO.reset(VertexArrayObject::CreateVertexArrayObject({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float3, "a_Color"		},
			{ ShaderDataType::Float2, "a_TexCoord"	}
 		}));
  		m_VAO->BindArray();

		m_VBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_VBO->BindBuffer();
		m_VBO->InitBuffer(VERTEX_ARRAY_SIZE_QUADS);

		m_VAO->EnableVertexAttribArray();

		// Init Circle rendering VAO, VBO
		m_CircleVAO.reset(VertexArrayObject::CreateVertexArrayObject({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float3, "a_Color"		},
			{ ShaderDataType::Float2, "a_TexCoord"	},
			{ ShaderDataType::Float,  "a_Thickness" },
			{ ShaderDataType::Float,  "a_Fade"		}
		}));
		m_CircleVAO->BindArray();

		m_CircleVBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_CircleVBO->BindBuffer();
		m_CircleVBO->InitBuffer(VERTEX_ARRAY_SIZE_CIRCLES);

		// Build VAO layout
		m_CircleVAO->EnableVertexAttribArray();

		// Init Line rendering VAO, VBO
		m_LineVAO.reset(VertexArrayObject::CreateVertexArrayObject({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float3, "a_Color"		}
			}));
		m_LineVAO->BindArray();

		m_LineVBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_LineVBO->BindBuffer();
		m_LineVBO->InitBuffer(VERTEX_ARRAY_SIZE_LINES);

		// Build VAO layout
		m_LineVAO->EnableVertexAttribArray();

		// Debug data
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;

		// Init Quad cmd storage
		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(m_VAO->VertexSize()));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;

		// Init Circle cmd storage
		m_CircleRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_CircleVAO->VertexSize()));
		m_CircleActiveRenderCmdIter = std::end(m_CircleRenderCmds) - 1;

		// Init Line cmd storage
		m_LineRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_LineVAO->VertexSize()));
		m_LineActiveRenderCmdIter = std::end(m_LineRenderCmds) - 1;

		m_CircleShader.reset(Shader::CreateShader("..\/assets\/shaders\/circle.shader"));
		m_LineShader.reset(Shader::CreateShader("..\/assets\/shaders\/line.shader"));
		// For local rendering of FBs
		m_RenderFrameBufferShader.reset(Shader::CreateShader("..\/assets\/shaders\/framebuffer.shader"));
		m_RenderFrameBufferCmd = std::make_shared<RenderCommand>(m_VAO->VertexSize());

		// Populate fixed data, shader uniform is set every frame
		m_RenderFrameBufferCmd->m_Texture = m_FrameBuffer->GetTexture();
		m_RenderFrameBufferCmd->m_Shader = m_RenderFrameBufferShader.get();

		SetLineWidth(DEFAULT_LINE_WIDTH);
	}

	Renderer2D::~Renderer2D()
	{
		delete m_RenderAPI;
		m_RenderAPI = nullptr;
	}

	Renderer2D* Renderer2D::Instance()
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
		m_ActiveCamera = camera;
	}

	void Renderer2D::EndScene()
	{
		Flush();
		FlushCircle();
		FlushLine();
	}
	
	void Renderer2D::Submit(glm::vec4 quad, Texture* texture, Shader* shader)
	{	
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, 0.0f, texture, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, SubTexture* subtexture, Shader* shader)
	{
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, 0.0f, subtexture, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, Shader* shader)
	{
		Submit(quad, (Texture*)nullptr, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, glm::vec3 color, Shader* shader)
	{
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, 0.0f, color, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, float rotation, Texture* texture, Shader* shader)
	{
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, rotation, texture, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, float rotation, SubTexture* subtexture, Shader* shader)
	{
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, rotation, subtexture, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, float rotation, Shader* shader)
	{
		Submit(quad, rotation, (Texture*)nullptr, shader);
	}

	void Renderer2D::Submit(glm::vec4 quad, float rotation, glm::vec3 color, Shader* shader)
	{
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f };
		glm::vec2 scale = { quad.z, quad.w };

		Submit(position, scale, rotation, color, shader);
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Texture* texture, Shader* shader)
	{
		CheckTextureBatching(texture);
		auto currnetCmd = *m_ActiveRenderCmdIter;

		CheckCapacity();
		// First time call Submit after BeginScene
		if ((currnetCmd->m_Texture == nullptr) && ((currnetCmd->m_Shader == nullptr)))
		{
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation);
			currnetCmd->m_Texture = texture;
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		}
		else if ((currnetCmd->m_Texture != texture) || (currnetCmd->m_Shader != shader))
		{
			// If shader or texture changed
			Flush();
			currnetCmd = *m_ActiveRenderCmdIter;
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation);
			currnetCmd->m_Texture = texture;
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		}
		else
		{
			// Batching vertex data
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation);
		}
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, SubTexture* subtexture, Shader* shader)
	{
		if (!subtexture)
			return;

		CheckTextureBatching(subtexture->GetTexture());
		auto currnetCmd = *m_ActiveRenderCmdIter;

		CheckCapacity();
		// First time call Submit after BeginScene
		if ((currnetCmd->m_Texture == nullptr) && ((currnetCmd->m_Shader == nullptr)))
		{
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation, glm::vec3{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
			currnetCmd->m_Texture = subtexture->GetTexture();
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		}
		else if ((currnetCmd->m_Texture != subtexture->GetTexture()) || (currnetCmd->m_Shader != shader))
		{
			// If shader or texture changed
			Flush();
			currnetCmd = *m_ActiveRenderCmdIter;
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation, glm::vec3{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
			currnetCmd->m_Texture = subtexture->GetTexture();
			currnetCmd->m_Shader = shader;

			shader->SetUniform("u_texture", 0);
		}
		else
		{
			// Batching vertex data
			PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation, glm::vec3{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
		}
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Shader* shader, glm::vec2 minUV/*{ 0.0f, 0.0f }*/, glm::vec2 maxUV/*{ 1.0f, 1.0f }*/)
	{
		Submit(pos, scale, rotation, (Texture*)nullptr, shader);
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec3 color, Shader* shader, glm::vec2 minUV/*{ 0.0f, 0.0f }*/, glm::vec2 maxUV/*{ 1.0f, 1.0f }*/)
	{
		auto currnetCmd = *m_ActiveRenderCmdIter;

		CheckCapacity();
		// Flush if we have a texture set from other Submits or shader changed
		if ((currnetCmd->m_Texture != nullptr) || (currnetCmd->m_Shader != shader))
		{
			Flush();
			currnetCmd = *m_ActiveRenderCmdIter;
		}

		PushTransformedVertexData(currnetCmd.get(), pos, scale, rotation, color, minUV, maxUV);
		currnetCmd->m_Shader = shader;
	}

	void Renderer2D::DrawCircle(glm::vec3 pos, glm::vec2 scale, glm::vec3 color, float thickness, float fade)
	{
		/*m_CircleShader->SetUniform("u_thickness", thickness);
		m_CircleShader->SetUniform("u_fade", fade);

		Submit(pos, scale, 0.0f, color, m_CircleShader.get(), { -1.0f, -1.0f }, { 1.0f, 1.0f });*/

		auto currnetCmd = *m_CircleActiveRenderCmdIter;
		auto cmd = currnetCmd.get();
		glm::vec2 minUV = { -1.0f, -1.0f };
		glm::vec2 maxUV = { 1.0f, 1.0f };

		CheckCapacity();

		// Set cmd data
		currnetCmd->m_Shader = m_CircleShader.get();

		// Apply transforms
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		cmd->PushCircleVertex(transform * s_QuadVertexPosition[0], color, { minUV.x, minUV.y }, thickness, fade);
		cmd->PushCircleVertex(transform * s_QuadVertexPosition[1], color, { maxUV.x, minUV.y }, thickness, fade);
		cmd->PushCircleVertex(transform * s_QuadVertexPosition[2], color, { maxUV.x, maxUV.y }, thickness, fade);

		cmd->PushCircleVertex(transform * s_QuadVertexPosition[2], color, { maxUV.x, maxUV.y }, thickness, fade);
		cmd->PushCircleVertex(transform * s_QuadVertexPosition[3], color, { minUV.x, maxUV.y }, thickness, fade);
		cmd->PushCircleVertex(transform * s_QuadVertexPosition[0], color, { minUV.x, minUV.y }, thickness, fade);
	}

	void Renderer2D::SetLineWidth(float width)
	{
		m_RenderAPI->SetLineWidth(width);
	}

	void Renderer2D::DrawLine(glm::vec3 p0, glm::vec3 p1, glm::vec3 color)
	{
		auto currentCmd = *m_LineActiveRenderCmdIter;
		auto cmd = currentCmd.get();

		CheckCapacity();

		// Set cmd data
		currentCmd->m_Shader = m_LineShader.get();

		cmd->PushLineVertex(p0, color);
		cmd->PushLineVertex(p1, color);
	}

	void Renderer2D::DrawQuad(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec3 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rotation/*glm::radians(rotation)*/, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		DrawLine(transform * s_QuadVertexPosition[0], transform * s_QuadVertexPosition[1], color);
		DrawLine(transform * s_QuadVertexPosition[1], transform * s_QuadVertexPosition[2], color);
		DrawLine(transform * s_QuadVertexPosition[2], transform * s_QuadVertexPosition[3], color);
		DrawLine(transform * s_QuadVertexPosition[3], transform * s_QuadVertexPosition[0], color);
	}

	void Renderer2D::Flush()
	{
		auto currnetCmd = *m_ActiveRenderCmdIter;

		if (currnetCmd->m_Shader == nullptr)
		{
			// No data to qualify as a valid RenderCommand
			return;
		}

		// For now set ViewProj camera uniform here
		currnetCmd->m_Shader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(m_VAO->VertexSize()));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;
	}

	void Renderer2D::FlushCircle()
	{
		auto currnetCmd = *m_CircleActiveRenderCmdIter;

		if (currnetCmd->m_Shader == nullptr)
		{
			// No data to qualify as a valid RenderCommand
			return;
		}

		// For now set ViewProj camera uniform here
		currnetCmd->m_Shader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_CircleRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_CircleVAO->VertexSize()));
		m_CircleActiveRenderCmdIter = std::end(m_CircleRenderCmds) - 1;
	}

	void Renderer2D::FlushLine()
	{
		auto currnetCmd = *m_LineActiveRenderCmdIter;

		if (currnetCmd->m_Shader == nullptr)
		{
			// No data to qualify as a valid RenderCommand
			return;
		}

		// For now set ViewProj camera uniform here
		currnetCmd->m_Shader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_LineRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_LineVAO->VertexSize()));
		m_LineActiveRenderCmdIter = std::end(m_LineRenderCmds) - 1;
	}

	void Renderer2D::FlushScene()
	{
		// Clear Render commands
		ClearRenderCommands();
	}

	void Renderer2D::RenderFrameBuffer()
	{
		m_RenderFrameBufferShader->SetUniform("u_texture", 0);
		m_RenderFrameBufferShader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_RenderFrameBufferCmd->ClearData();
		PushQuadVertexData(m_RenderFrameBufferCmd.get(), glm::vec4(0.0f, 0.0f, GetCanvasWidth(), GetCanvasHeight()));

		DrawRenderCmd(m_RenderFrameBufferCmd);
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
		for (auto renderCmd : m_RenderCmds)
		{
			// Skip last as it won't be filled with data
			if (renderCmd != *m_ActiveRenderCmdIter)
				DrawRenderCmd(renderCmd);
		}
		m_RenderedCmdCnt = m_RenderCmds.size() - 1; // last won't be filled with data

		for (auto renderCmd : m_CircleRenderCmds)
		{
			// Skip last as it won't be filled with data
			if (renderCmd != *m_CircleActiveRenderCmdIter)
				DrawCirlceRenderCmd(renderCmd);
		}
		m_RenderedCmdCnt += m_CircleRenderCmds.size() - 1;

		for (auto renderCmd : m_LineRenderCmds)
		{
			// Skip last as it won't be filled with data
			if (renderCmd != *m_LineActiveRenderCmdIter)
				DrawLineRenderCmd(renderCmd);
		}
		m_RenderedCmdCnt += m_LineRenderCmds.size() - 1;
	}

	void Renderer2D::EndFrame()
	{
		// TODO: support multiple render targets here (other FBO)
		// Unbind stuff
		m_FrameBuffer->Unbind();
		m_RenderAPI->UnbindTexture();

		if (m_LocalRendering)	// If app handles rendering FB locally to window
			RenderFrameBuffer();

		// Clear Render commands
		ClearRenderCommands();

		// Clear Debug data
		ShowDebugData();
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;
	}

	void Renderer2D::DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
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

	void Renderer2D::DrawCirlceRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto shader = renderCmd->m_Shader;

		shader->Use();
		shader->SetBufferedUniforms();

		m_CircleVAO->BindArray();
		m_CircleVBO->BindBuffer();

		m_CircleVBO->BufferSubData(0, renderCmd->Size(), renderCmd->CircleData());
		m_RenderAPI->DrawArrays(DrawElementsMode::TRIANGLES, 0, renderCmd->m_VertexCount);
		m_DrawCallCnt++;

		shader->ClearBufferedUniforms();
	}

	void Renderer2D::DrawLineRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto shader = renderCmd->m_Shader;

		shader->Use();
		shader->SetBufferedUniforms();

		m_LineVAO->BindArray();
		m_LineVBO->BindBuffer();

		m_LineVBO->BufferSubData(0, renderCmd->Size(), renderCmd->LineData());
		m_RenderAPI->DrawArrays(DrawElementsMode::LINES, 0, renderCmd->m_VertexCount);
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
		auto currnetCmd = *m_ActiveRenderCmdIter;
		auto currentCricleCmd = *m_CircleActiveRenderCmdIter;
		auto currentLineCmd = *m_LineActiveRenderCmdIter;

		if (currnetCmd->m_VertexCount >= VERTEX_CMD_CAPACITY)
		{
			Flush();
		}

		if (currentCricleCmd->m_VertexCount >= VERTEX_CMD_CAPACITY)
		{
			FlushCircle();
		}

		if (currentLineCmd->m_VertexCount >= VERTEX_CMD_CAPACITY)
		{
			FlushLine();
		}
	}

	void Renderer2D::CheckTextureBatching(Texture* texture)
	{
		auto found = std::find_if(std::begin(m_RenderCmds), std::end(m_RenderCmds), [&texture](auto elem) -> bool {
			if (texture != nullptr && elem->m_Texture != nullptr)
				return texture->GetTextureID() == elem->m_Texture->GetTextureID();
			else
				return false;
			});

		if (found != std::end(m_RenderCmds))
		{
			m_ActiveRenderCmdIter = found;
		}
	}

	void Renderer2D::ClearRenderCommands()
	{
		// Clear Render commands
		m_RenderCmds.clear();
		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(m_VAO->VertexSize()));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;

		m_CircleRenderCmds.clear();
		m_CircleRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_CircleVAO->VertexSize()));
		m_CircleActiveRenderCmdIter = std::end(m_CircleRenderCmds) - 1;

		m_LineRenderCmds.clear();
		m_LineRenderCmds.emplace_back(std::make_shared<RenderCommand>(m_LineVAO->VertexSize()));
		m_LineActiveRenderCmdIter = std::end(m_LineRenderCmds) - 1;
	}

	void Renderer2D::PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec3 color /*= { 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{	
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f};
		glm::vec2 size = { quad.z, quad.w };

		PushTransformedVertexData(cmd, position, size, 0.0f, color, min, max);
	}

	void Renderer2D::PushTransformedVertexData(RenderCommand* cmd, glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec3 color /*= { 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rotation/*glm::radians(rotation)*/, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		cmd->PushVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
		cmd->PushVertex(transform * s_QuadVertexPosition[1], color, { max.x, min.y });
		cmd->PushVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });

		cmd->PushVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });
		cmd->PushVertex(transform * s_QuadVertexPosition[3], color, { min.x, max.y });
		cmd->PushVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
	}
}