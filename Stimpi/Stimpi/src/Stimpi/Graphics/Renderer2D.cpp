#include "stpch.h"
#include "Stimpi/Graphics/Renderer2D.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Window.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/ShaderImporter.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/VisualScripting/ExecTree.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

/**
 * TODO: Consider making shader usage more flexible. Adding custom shader usage.
 * FIXME: Custom shader can fix pixel wobble effect (moving objects). 
 *   Cam.size / FB.siza (canvas) will be rounding factor or fix in shader.
 */

/**
 * Custom shader shall have default layer data: Pos/Color/TexCoord 3/4/2
 */

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
		WindowConfiguration windowConfig = Window::GetWindowConfiguration();
		m_FrameBuffer.reset(FrameBuffer::CreateFrameBuffer({ windowConfig.m_WindowWidth, windowConfig.m_WindowHeight, 4/*Color Channels*/}));

		// Init Quad rendering VAO, VBO
		m_QuadVAO.reset(VertexArrayObject::CreateVertexArrayObject({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float4, "a_Color"		},
			{ ShaderDataType::Float2, "a_TexCoord"	}
 		}));
  		m_QuadVAO->BindArray();

		m_QuadVBO.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));
		m_QuadVBO->BindBuffer();
		m_QuadVBO->InitBuffer(VERTEX_ARRAY_SIZE_QUADS);

		m_QuadVAO->EnableVertexAttribArray();

		// Init Circle rendering VAO, VBO
		m_CircleVAO.reset(VertexArrayObject::CreateVertexArrayObject({
			{ ShaderDataType::Float3, "a_Position"	},
			{ ShaderDataType::Float4, "a_Color"		},
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
			{ ShaderDataType::Float4, "a_Color"		}
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

		// Init render cmd storage
		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(0));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;

		m_CircleShader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/circle.shader");
		m_LineShader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/line.shader");
		// For local rendering of FBs
		m_RenderFrameBufferShader = ShaderImporter::LoadShader(Project::GetResourcesDir() / "shaders\/framebuffer.shader");
		m_RenderFrameBufferCmd = std::make_shared<RenderCommand>(m_QuadVAO->VertexSize());
		m_FramebufferCamera = std::make_shared<OrthoCamera>(0.0f, 1280.0f, 0.0f, 720.0f);

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
		FlushScene();
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Texture* texture, Shader* shader)
	{
		CheckTextureBatching(texture);
		auto currentCmd = *m_ActiveRenderCmdIter;

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::QUAD && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
			currentCmd->m_Texture = texture;
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}

		CheckCapacity();
		// First time call Submit after BeginScene
		if ((currentCmd->m_Texture == nullptr) && ((currentCmd->m_Shader == nullptr)))
		{
			PushTransformedVertexData(currentCmd.get(), pos, scale, rotation);
			currentCmd->m_Texture = texture;
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}
		else if ((currentCmd->m_Texture != texture) || (currentCmd->m_Shader != shader))
		{
			// If shader or texture changed
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
			PushTransformedVertexData(currentCmd.get(), pos, scale, rotation);
			currentCmd->m_Texture = texture;
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}
		// TODO: custom shader handling - check if "custom" data changed
		else
		{
			// Batching vertex data
			PushTransformedVertexData(currentCmd.get(), pos, scale, rotation);
		}
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, SubTexture* subtexture, Shader* shader)
	{
		if (!subtexture)
			return;

		CheckTextureBatching(subtexture->GetTexture());
		auto currentCmd = (*m_ActiveRenderCmdIter).get();

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::QUAD && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = (*m_ActiveRenderCmdIter).get();
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}

		CheckCapacity();
		// First time call Submit after BeginScene
		if ((currentCmd->m_Texture == nullptr) && ((currentCmd->m_Shader == nullptr)))
		{
			PushTransformedVertexData(currentCmd, pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}
		else if ((currentCmd->m_Texture != subtexture->GetTexture()) || (currentCmd->m_Shader != shader))
		{
			// If shader or texture changed
			NewCmd();
			currentCmd = (*m_ActiveRenderCmdIter).get();
			PushTransformedVertexData(currentCmd, pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader;

			SetShaderUniforms(shader);
		}
		else
		{
			// Batching vertex data
			PushTransformedVertexData(currentCmd, pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
		}
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Shader* shader, glm::vec2 minUV/*{ 0.0f, 0.0f }*/, glm::vec2 maxUV/*{ 1.0f, 1.0f }*/)
	{
		Submit(pos, scale, rotation, (Texture*)nullptr, shader);
	}

	void Renderer2D::Submit(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color, Shader* shader, glm::vec2 minUV/*{ 0.0f, 0.0f }*/, glm::vec2 maxUV/*{ 1.0f, 1.0f }*/)
	{
		CheckShaderBatching(shader);
		auto currentCmd = *m_ActiveRenderCmdIter;

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::QUAD && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
		}

		CheckCapacity();
		// Flush if we have a texture set from other Submits or shader changed
		if ((currentCmd->m_Texture != nullptr) || (currentCmd->m_Shader != shader))
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
		}

		PushTransformedVertexData(currentCmd.get(), pos, scale, rotation, color, minUV, maxUV);
		currentCmd->m_Shader = shader;
	}

	void Renderer2D::SubmitCircle(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, float thickness, float fade)
	{
		CheckBatchingByType(RenderCommandType::CIRLCE);
		auto currentCmd = *m_ActiveRenderCmdIter;
		glm::vec2 minUV = { -1.0f, -1.0f };
		glm::vec2 maxUV = { 1.0f, 1.0f };

		CheckCapacity();

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::CIRLCE && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
		}

		// Set cmd data
		currentCmd->m_Shader = m_CircleShader.get();

		// Apply transforms
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		if (currentCmd->m_VertexSize == 0)
			currentCmd->m_VertexSize = m_CircleVAO->VertexSize();

		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[0], color, { minUV.x, minUV.y }, thickness, fade);
		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[1], color, { maxUV.x, minUV.y }, thickness, fade);
		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[2], color, { maxUV.x, maxUV.y }, thickness, fade);

		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[2], color, { maxUV.x, maxUV.y }, thickness, fade);
		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[3], color, { minUV.x, maxUV.y }, thickness, fade);
		currentCmd->PushCircleVertex(transform * s_QuadVertexPosition[0], color, { minUV.x, minUV.y }, thickness, fade);
	}

	void Renderer2D::SetLineWidth(float width)
	{
		m_RenderAPI->SetLineWidth(width);
	}

	void Renderer2D::SubmitLine(glm::vec3 p0, glm::vec3 p1, glm::vec4 color)
	{
		CheckBatchingByType(RenderCommandType::LINE);
		auto currentCmd = *m_ActiveRenderCmdIter;

		CheckCapacity();

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::LINE && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
		}

		// Set cmd data
		currentCmd->m_Shader = m_LineShader.get();

		if (currentCmd->m_VertexSize == 0)
			currentCmd->m_VertexSize = m_LineVAO->VertexSize();

		currentCmd->PushLineVertex(p0, color);
		currentCmd->PushLineVertex(p1, color);
	}

	void Renderer2D::SubmitCustom(glm::vec3 pos, glm::vec2 scale, float rotation, SubTexture* subtexture, Material* material, Entity entity)
	{
		if (!subtexture || !material)
			return;

		CheckMaterialBatching(material, subtexture->GetTexture());
		auto currentCmd = *m_ActiveRenderCmdIter;
		auto shader = material->GetShader();

		// Check if some other type was used in active cmd
		if (currentCmd->m_Type != RenderCommandType::VARIABLE && currentCmd->m_Type != RenderCommandType::NONE)
		{
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader.get();
			currentCmd->m_Material = material;

			// Sets texture index for shader to use
			SetShaderUniforms(shader.get());
		}

		CheckCapacity();

		// First time call Submit after BeginScene
		if ((currentCmd->m_Texture == nullptr) && ((currentCmd->m_Material == nullptr)))
		{
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader.get();
			currentCmd->m_Material = material;
			PushTransformedCustomVertexData(entity, currentCmd.get(), pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());

			SetShaderUniforms(shader.get());
		}
		else if ((currentCmd->m_Texture != subtexture->GetTexture()) || (currentCmd->m_Material != material))
		{
			// If shader or texture changed
			NewCmd();
			currentCmd = *m_ActiveRenderCmdIter;
			currentCmd->m_Texture = subtexture->GetTexture();
			currentCmd->m_Shader = shader.get();
			currentCmd->m_Material = material;
			PushTransformedCustomVertexData(entity, currentCmd.get(), pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());

			SetShaderUniforms(shader.get());
		}
		else
		{
			// Batching vertex data
			PushTransformedCustomVertexData(entity, currentCmd.get(), pos, scale, rotation, glm::vec4{ 1.0f }, subtexture->GetUVMin(), subtexture->GetUVMax());
		}
	}

	void Renderer2D::SubmitSquare(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rotation/*glm::radians(rotation)*/, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		SubmitLine(transform * s_QuadVertexPosition[0], transform * s_QuadVertexPosition[1], color);
		SubmitLine(transform * s_QuadVertexPosition[1], transform * s_QuadVertexPosition[2], color);
		SubmitLine(transform * s_QuadVertexPosition[2], transform * s_QuadVertexPosition[3], color);
		SubmitLine(transform * s_QuadVertexPosition[3], transform * s_QuadVertexPosition[0], color);
	}

	void Renderer2D::NewCmd()
	{
		auto currnetCmd = *m_ActiveRenderCmdIter;

		if (currnetCmd->m_Shader == nullptr)
		{
			// No data to qualify as a valid RenderCommand
			return;
		}
		
		// For now set ViewProj camera uniform here
		currnetCmd->m_Shader->SetUniform("u_ViewProjection", m_ActiveCamera->GetViewProjectionMatrix());

		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(0));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;
	}


	void Renderer2D::FlushScene()
	{
		NewCmd();
		DrawFrame();
		// Clear current Render commands
		ClearRenderCommands();
	}

	void Renderer2D::ClearScene()
	{
		// Clear Render commands
		ClearRenderCommands();
	}

	void Renderer2D::RenderFrameBuffer()
	{
		auto width = m_FrameBuffer->GetWidth();
		auto height = m_FrameBuffer->GetHeight();
		auto camView = m_ActiveCamera->GetViewQuad();
		auto zoom = m_ActiveCamera->GetZoom();
		float texelPerPixel = camView.w / m_FrameBuffer->GetHeight() / zoom;  // camView.y - width

		m_RenderFrameBufferShader->SetUniform("u_TexelSize", glm::vec4(1.0f / width, 1.0f / height, (float)width, (float)height));
		m_RenderFrameBufferShader->SetUniform("u_TexelsPerPixel", texelPerPixel);
		m_RenderFrameBufferShader->SetUniform("u_ViewProjection", m_FramebufferCamera->GetViewProjectionMatrix());

		m_RenderFrameBufferCmd->ClearData();
		PushQuadVertexData(m_RenderFrameBufferCmd.get(), glm::vec4(0.0f, 0.0f, GetCanvasWidth(), GetCanvasHeight()));

		DrawQuadRenderCmd(m_RenderFrameBufferCmd);
	}


	void Renderer2D::SetShaderUniforms(Shader* shader)
	{
		auto currnetCmd = *m_ActiveRenderCmdIter;

		if (shader == nullptr)
			return;

		shader->SetUniform("u_texture", 0);
	}

	void Renderer2D::ResizeCanvas(uint32_t width, uint32_t height)
	{
		m_FrameBuffer->Resize(width, height);

		if (m_LocalRendering)
		{
			m_FramebufferCamera->Resize(0.0, width, 0.0f, height);
		}
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
		for (auto& renderCmd : m_RenderCmds)
		{
			// Skip last as it won't be filled with data
			if (renderCmd != *m_ActiveRenderCmdIter)
			{
				if (renderCmd->m_Type == RenderCommandType::QUAD)
					DrawQuadRenderCmd(renderCmd);
				else if (renderCmd->m_Type == RenderCommandType::CIRLCE)
					DrawCirlceRenderCmd(renderCmd);
				else if (renderCmd->m_Type == RenderCommandType::LINE)
					DrawLineRenderCmd(renderCmd);
				else if (renderCmd->m_Type == RenderCommandType::VARIABLE)
					DrawVariableRenderCmd(renderCmd);
			}
		}
		m_RenderedCmdCnt += m_RenderCmds.size() - 1;
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
		m_LastFrameDrawCallCnt = m_DrawCallCnt;
		m_LastFrameRenderedCmdCnt = m_RenderedCmdCnt;
		m_DrawCallCnt = 0;
		m_RenderedCmdCnt = 0;
	}

	void Renderer2D::RegisterShader(AssetHandle shaderHandle)
	{
		auto shader = AssetManager::GetAsset<Shader>(shaderHandle);
		auto& info = shader->GetInfo();
		unsigned int shaderID = shader->GetShaderID();

		// Check if PrepareCustomShaderObject was already performed for the registering shader
		if (std::find(m_CustomShaders.begin(), m_CustomShaders.end(), shaderHandle) == m_CustomShaders.end())
		{
			VertexBufferLayout vboLayout(info.m_ShaderLayout.m_Data);

			BuildCustomVAO(vboLayout, shaderID);
			BuildCustomVBO(BufferObjectType::ARRAY_BUFFER, shaderID);
			PrepareCustomShaderObjects(shaderID);

			m_CustomShaders.push_back(shaderHandle);
		}
		else
		{
			ST_CORE_WARN("RegisterShader: Shader [ID={}] already registered!", shaderID);
		}
	}

	void Renderer2D::UnregisterShader(AssetHandle shaderHandle)
	{
		auto shader = AssetManager::GetAsset<Shader>(shaderHandle);
		unsigned int shaderID = shader->GetShaderID();
		auto vao = m_CustomVAOs[shaderID];
		auto vbo = m_CustomVBOs[shaderID];

		if (vao != nullptr && vbo != nullptr)
		{
			vao->Unbind();
			vbo->Unbind();

			m_CustomVAOs.erase(shaderID);
			m_CustomVBOs.erase(shaderID);
		}

		auto found = std::find(m_CustomShaders.begin(), m_CustomShaders.end(), shaderHandle);
		if (found != m_CustomShaders.end())
			m_CustomShaders.erase(found);
	}

	void Renderer2D::DrawQuadRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto shader = renderCmd->m_Shader;
		// In case we are using Material
		if (renderCmd->m_Material != nullptr)
		{
			// We need to set uniforms now
			for (auto& uni : shader->GetInfo().m_Uniforms)
			{
				auto values = renderCmd->m_Material->GetUniformValues();
				shader->SetUniform(uni.m_Name, values.at(uni.m_Name));
			}
		}

		shader->Use();
		shader->SetBufferedUniforms();

		m_QuadVAO->BindArray();
		m_QuadVBO->BindBuffer();

		if (renderCmd->m_Texture != nullptr)
			renderCmd->m_Texture->UseTexture();

		m_QuadVBO->BufferSubData(0, renderCmd->Size(), renderCmd->Data());
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

	void Renderer2D::DrawVariableRenderCmd(std::shared_ptr<RenderCommand>& renderCmd)
	{
		auto shader = renderCmd->m_Shader;
		auto shaderID = shader->GetShaderID();
		// In case we are using Material
		if (renderCmd->m_Material != nullptr)
		{
			// We need to set uniforms now
			for (auto& uni : shader->GetInfo().m_Uniforms)
			{
				auto values = renderCmd->m_Material->GetUniformValues();
				shader->SetUniform(uni.m_Name, values.at(uni.m_Name));
			}
		}

		shader->Use();
		shader->SetBufferedUniforms();

		auto customVAO = m_CustomVAOs.at(shaderID);
		auto customVBO = m_CustomVBOs.at(shaderID);

		customVAO->BindArray();
		customVBO->BindBuffer();

		if (renderCmd->m_Texture != nullptr)
			renderCmd->m_Texture->UseTexture();

		customVBO->BufferSubData(0, renderCmd->Size(), renderCmd->VariableData());
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

	unsigned int Renderer2D::BuildCustomVAO(const VertexBufferLayout& layout, const unsigned int& shaderID)
	{
		std::shared_ptr<VertexArrayObject> vao;
		vao.reset(VertexArrayObject::CreateVertexArrayObject(layout));

		if (m_CustomVAOs.find(shaderID) == m_CustomVAOs.end())
		{
			m_CustomVAOs[shaderID] = vao;
		}
		else
		{
			ST_CORE_WARN("BuildCustomVAO: Shader [ID={}] already has registered VertexArrayObject!", shaderID);
		}

		return vao->GetID();
	}

	unsigned int Renderer2D::BuildCustomVBO(const BufferObjectType& type, const unsigned int& shaderID)
	{
		std::shared_ptr<BufferObject> vbo;
		vbo.reset(BufferObject::CreateBufferObject(BufferObjectType::ARRAY_BUFFER));

		if (m_CustomVBOs.find(shaderID) == m_CustomVBOs.end())
		{
			m_CustomVBOs[shaderID] = vbo;
		}
		else
		{
			ST_CORE_WARN("BuildCustomVBO: Shader [ID={}] already has registered BufferObject!", shaderID);
		}

		return vbo->GetID();
	}

	void Renderer2D::PrepareCustomShaderObjects(const unsigned int& shaderID)
	{
		auto vao = m_CustomVAOs[shaderID];
		auto vbo = m_CustomVBOs[shaderID];

		if (vao != nullptr && vbo != nullptr)
		{
			vao->BindArray();
			vbo->BindBuffer();
			vbo->InitBuffer(VERTEX_ARRAY_SIZE_QUADS);
			vao->EnableVertexAttribArray();
		}
		else
		{
			ST_CORE_WARN("PrepareCustomShader: Shader [ID={}] has no valid VAO or BO created!", shaderID);
		}
	}

	void Renderer2D::CheckCapacity()
	{
		auto currentCmd = *m_ActiveRenderCmdIter;
		if (currentCmd->m_VertexCount >= VERTEX_CMD_CAPACITY)
			NewCmd();
	}	

	// TODO: when looking for valid batching cmd, consider max vertex capacity
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

	// TODO: when looking for valid batching cmd, consider max vertex capacity
	void Renderer2D::CheckShaderBatching(Shader* shader)
	{
		auto found = std::find_if(std::begin(m_RenderCmds), std::end(m_RenderCmds), [&shader](auto elem) -> bool {
			if (shader != nullptr && elem->m_Shader != nullptr && elem->m_Texture == nullptr)
				return shader == elem->m_Shader;
			else
				return false;
			});

		if (found != std::end(m_RenderCmds))
		{
			m_ActiveRenderCmdIter = found;
		}
	}

	void Renderer2D::CheckMaterialBatching(Material* material, Texture* texture)
	{
		auto found = std::find_if(std::begin(m_RenderCmds), std::end(m_RenderCmds), [&material, &texture](auto elem) -> bool {
			// 1. Has the texture changed?
			if (texture != elem->m_Texture)
				return false;

			// 2. Has the shader changed?
			if (material != elem->m_Material)
				return false;
			else
			{
				// 3. Has any of the shader uniforms changed?
				for (auto& it : material->GetUniformValues())
				{
					if (it.second != elem->m_Material->GetUniformValues().at(it.first))
						return false;
				}

				return true;
			}

			});

		if (found != std::end(m_RenderCmds))
		{
			m_ActiveRenderCmdIter = found;
		}
	}

	// TODO: when looking for valid batching cmd, consider max vertex capacity
	void Renderer2D::CheckBatchingByType(RenderCommandType type)
	{
		auto found = std::find_if(std::begin(m_RenderCmds), std::end(m_RenderCmds), [&type](auto elem) -> bool {
			if (elem->m_Type == type)
				return true;
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
		m_RenderCmds.clear();
		m_RenderCmds.emplace_back(std::make_shared<RenderCommand>(0));
		m_ActiveRenderCmdIter = std::end(m_RenderCmds) - 1;

	}

	void Renderer2D::PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec4 color /*= { 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{	
		glm::vec3 position = { quad.x + quad.z / 2.0f, quad.y + quad.w / 2.0f, 0.0f};
		glm::vec2 size = { quad.z, quad.w };

		PushTransformedVertexData(cmd, position, size, 0.0f, color, min, max);
	}

	void Renderer2D::PushTransformedVertexData(RenderCommand* cmd, glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color /*= { 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rotation/*glm::radians(rotation)*/, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		if (cmd->m_VertexSize == 0)
			cmd->m_VertexSize = m_QuadVAO->VertexSize();

		cmd->PushQuadVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
		cmd->PushQuadVertex(transform * s_QuadVertexPosition[1], color, { max.x, min.y });
		cmd->PushQuadVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });

		cmd->PushQuadVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });
		cmd->PushQuadVertex(transform * s_QuadVertexPosition[3], color, { min.x, max.y });
		cmd->PushQuadVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
	}

	void Renderer2D::PushTransformedCustomVertexData(Entity entity, RenderCommand* cmd, glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/, glm::vec2 min /*= { 0.0f, 0.0f }*/, glm::vec2 max /*= { 1.0f, 1.0f }*/)
	{
		VariableVertexData varData;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		unsigned int shaderID = cmd->m_Shader->GetShaderID();

		if (cmd->m_VertexSize == 0)
			cmd->m_VertexSize = m_CustomVAOs.at(shaderID)->VertexSize();

		// 1. Find SetShaderData node
		auto execTree = cmd->m_Shader->GetCustomExecTree();
		if (execTree)
		{
			auto foundShaderDataNode = std::find_if(execTree->m_Methods.begin(), execTree->m_Methods.end(), [](std::shared_ptr<Method>& item)
				{ 
					return item->m_Name == MethodName::SetShaderData;
				});

			// 2. Execute the tree to calculate outputs
			if (foundShaderDataNode != execTree->m_Methods.end())
			{
				if (execTree->ExecuteWalk(entity))
				{
					// 3. Read outputs
					std::shared_ptr<Method> method = *foundShaderDataNode;
					for (auto& inIndex : method->m_InParams)
					{
						varData.m_Data.push_back(std::get<float>(execTree->m_Params[inIndex]));
					}
				}
				else
				{
					varData.m_Data.assign(cmd->m_VertexSize - 36, 0.0f);
				}
			}
		}
			
		// Vertex count is updated only in this version - TODO: find better solution
		cmd->PushVariableVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
		cmd->PushVariableVertex(varData);
		cmd->PushVariableVertex(transform * s_QuadVertexPosition[1], color, { max.x, min.y });
		cmd->PushVariableVertex(varData);
		cmd->PushVariableVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });
		cmd->PushVariableVertex(varData);

		cmd->PushVariableVertex(transform * s_QuadVertexPosition[2], color, { max.x, max.y });
		cmd->PushVariableVertex(varData);
		cmd->PushVariableVertex(transform * s_QuadVertexPosition[3], color, { min.x, max.y });
		cmd->PushVariableVertex(varData);
		cmd->PushVariableVertex(transform * s_QuadVertexPosition[0], color, { min.x, min.y });
		cmd->PushVariableVertex(varData);
	}

}