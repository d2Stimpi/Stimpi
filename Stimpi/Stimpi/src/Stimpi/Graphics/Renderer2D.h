#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/RenderAPI.h"
#include "Stimpi/Graphics/RenderCommand.h"

#include "Stimpi/Graphics/RenderCommand.h"

//TODO: combine in one header
#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/OrthoCamera.h"
#include "Stimpi/Graphics/FrameBuffer.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/SubTexture.h"
#include "Stimpi/Graphics/VertexArrayObject.h"

#define VERTEX_CMD_CAPACITY			(6 * 10000) // 6 vertex per quad, 10k quads
#define VERTEX_ARRAY_SIZE_QUADS		(192 * 1000) // Layout{3,3,2} quad szie * 1000 (max 1k squares per call)  
#define VERTEX_ARRAY_SIZE_CIRCLES	(240 * 1000) // Layout{3,3,2,1,1} cirlce szie * 1000 (max 1k squares per call)  
#define VERTEX_ARRAY_SIZE_LINES		(144 * 1000) // Layout{3,3} line szie * 1000 (max 1k squares per call)  
#define RENDERER_DBG	(true)
#define DEFAULT_LINE_WIDTH	(3.0f)

namespace Stimpi
{
	class ST_API Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		static Renderer2D* Instance();

		void EnableLocalRendering(bool enable) { m_LocalRendering = enable; };
		bool IsLocalRendering() { return m_LocalRendering; }

		void BeginScene(OrthoCamera* camera);
		void EndScene();

		// Cancel all render commands
		void ClearScene();

		// Rendering by Transforms
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Texture* texture, Shader* shader);
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, SubTexture* subtexture, Shader* shader);
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Shader* shader, glm::vec2 minUV = { 0.0f, 0.0f }, glm::vec2 maxUV = { 1.0f, 1.0f });
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color, Shader* shader, glm::vec2 minUV = { 0.0f, 0.0f }, glm::vec2 maxUV = { 1.0f, 1.0f });

		void SubmitCircle(glm::vec3 pos, glm::vec2 scale, glm::vec4 color, float thickness, float fade);
		void SubmitSquare(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color);
		void SubmitLine(glm::vec3 p0, glm::vec3 p1, glm::vec4 color);

		void SetLineWidth(float width);

		// Event Callbacks
		void ResizeCanvas(uint32_t width, uint32_t height);

		// Canvas - FB size
		uint32_t GetCanvasWidth() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetWidth(); };
		uint32_t GetCanvasHeight() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetHeight(); };

		// Render queued stuff
		void StartFrame();
		void DrawFrame();
		void EndFrame();

		// Custom shader handling
		void RegisterShader(std::shared_ptr<Shader> shader);
		void UnregisterShader(std::shared_ptr<Shader> shader);

		// To link with ImGui
		FrameBuffer* GetFrameBuffer();

		uint32_t GetFrameDrawCallCount() { return m_LastFrameDrawCallCnt; }
		uint32_t GetFrameRednerCmdCount() { return m_LastFrameRenderedCmdCnt; }
	private:
		void NewCmd();
		void FlushScene();
		void RenderFrameBuffer(); // Used for Application to handle displaying of FBs ourselves

		void SetShaderUniforms(Shader* shader);

		void PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2 min = { 0.0f, 0.0f }, glm::vec2 max = { 1.0f, 1.0f });
		void PushTransformedVertexData(RenderCommand* cmd, glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2 min = { 0.0f, 0.0f }, glm::vec2 max = { 1.0f, 1.0f });
		
		void DrawQuadRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);
		void DrawCirlceRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);
		void DrawLineRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);

		void CheckCapacity();
		void CheckTextureBatching(Texture* texture);
		void CheckShaderBatching(Shader* shader);
		void CheckBatchingByType(RenderCommandType type);

		void ClearRenderCommands();

		void ShowDebugData();

		unsigned int BuildCustomVAO(const VertexBufferLayout& layout, const unsigned int& shaderID);
		unsigned int BuildCustomVBO(const BufferObjectType& type, const unsigned int& shaderID);
		void PrepareCustomShaderObjects(const unsigned int& shaderID);
	private:
		bool m_LocalRendering = false;

		RenderAPI* m_RenderAPI;
		OrthoCamera* m_ActiveCamera;

		// Custom shader rendering [shaderID : VAO] and [shaderID : VBO]
		std::unordered_map<unsigned int, std::shared_ptr<VertexArrayObject>> m_CustomVAOs;
		std::unordered_map<unsigned int, std::shared_ptr<BufferObject>> m_CustomVBOs;
		std::vector<std::shared_ptr<Shader>> m_CustomShaders;

		// Quad rendering
		std::shared_ptr<VertexArrayObject> m_QuadVAO;
		std::shared_ptr<BufferObject> m_QuadVBO;

		// Circle rendering
		std::shared_ptr<VertexArrayObject> m_CircleVAO;
		std::shared_ptr<BufferObject> m_CircleVBO;
		std::shared_ptr<Shader> m_CircleShader;

		// Line rendering
		std::shared_ptr<VertexArrayObject> m_LineVAO;
		std::shared_ptr<BufferObject> m_LineVBO;
		std::shared_ptr<Shader> m_LineShader;

		// Rendering cmd (quads, circles, lines)
		std::vector<std::shared_ptr<RenderCommand>> m_RenderCmds;
		std::vector<std::shared_ptr<RenderCommand>>::iterator m_ActiveRenderCmdIter;

		// For local rendering
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<OrthoCamera> m_FramebufferCamera;
		std::shared_ptr<Shader> m_RenderFrameBufferShader;
		std::shared_ptr<RenderCommand> m_RenderFrameBufferCmd;

		// Debug data per frame
		uint32_t m_DrawCallCnt;
		uint32_t m_RenderedCmdCnt;
		uint32_t m_LastFrameDrawCallCnt;
		uint32_t m_LastFrameRenderedCmdCnt;
	};
}