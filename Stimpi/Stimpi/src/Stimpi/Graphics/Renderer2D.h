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
#define VERTEX_ARRAY_SIZE_CIRCLES	(240 * 1000) // Layout{3,3,2,1,1} quad szie * 1000 (max 1k squares per call)  
#define RENDERER_DBG	(true)

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
		void FlushScene();
		
		// Quad rendering - TODO: to be removed
		void Submit(glm::vec4 quad, Texture* texture, Shader* shader);
		void Submit(glm::vec4 quad, SubTexture* subtexture, Shader* shader);
		void Submit(glm::vec4 quad, Shader* shader);
		void Submit(glm::vec4 quad, glm::vec3 color, Shader* shader);

		// Rotating Quad rendering
		void Submit(glm::vec4 quad, float rotation, Texture* texture, Shader* shader);
		void Submit(glm::vec4 quad, float rotation, SubTexture* subtexture, Shader* shader);
		void Submit(glm::vec4 quad, float rotation, Shader* shader);
		void Submit(glm::vec4 quad, float rotation, glm::vec3 color, Shader* shader);

		// TODO: SubTexture rendering - UVs
		// Rendering by Transforms
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Texture* texture, Shader* shader);
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, SubTexture* subtexture, Shader* shader);
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, Shader* shader, glm::vec2 minUV = { 0.0f, 0.0f }, glm::vec2 maxUV = { 1.0f, 1.0f });
		void Submit(glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec3 color, Shader* shader, glm::vec2 minUV = { 0.0f, 0.0f }, glm::vec2 maxUV = { 1.0f, 1.0f });

		// TODO: maybe add stuff like DrawQuad

		// Circle shape rendering
		void DrawCircle(glm::vec3 pos, glm::vec2 scale, glm::vec3 color, float thickness, float fade);

		// Event Callbacks
		void ResizeCanvas(uint32_t width, uint32_t height);

		// Canvas - FB size
		uint32_t GetCanvasWidth() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetWidth(); };
		uint32_t GetCanvasHeight() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetHeight(); };

		// Render queued stuff
		void StartFrame();
		void DrawFrame();
		void EndFrame();

		// To link with ImGui
		FrameBuffer* GetFrameBuffer();

	private:
		void Flush();
		void FlushCircle();
		void RenderFrameBuffer(); // Used for Application to handle displaying of FBs ourselves

		void PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec3 color = { 1.0f, 1.0f, 1.0f }, glm::vec2 min = { 0.0f, 0.0f }, glm::vec2 max = { 1.0f, 1.0f });
		void PushTransformedVertexData(RenderCommand* cmd, glm::vec3 pos, glm::vec2 scale, float rotation, glm::vec3 color = { 1.0f, 1.0f, 1.0f }, glm::vec2 min = { 0.0f, 0.0f }, glm::vec2 max = { 1.0f, 1.0f });
		
		void DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);
		void DrawCirlceRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);

		void CheckCapacity();
		void CheckTextureBatching(Texture* texture);

		void ClearRenderCommands();

		void ShowDebugData();
	private:
		bool m_LocalRendering = false;

		RenderAPI* m_RenderAPI;
		OrthoCamera* m_ActiveCamera;

		// Quad rendering
		std::shared_ptr<VertexArrayObject> m_VAO;
		std::shared_ptr<BufferObject> m_VBO;
		std::vector<std::shared_ptr<RenderCommand>> m_RenderCmds;
		std::vector<std::shared_ptr<RenderCommand>>::iterator m_ActiveRenderCmdIter;

		// Circle rendering
		std::shared_ptr<VertexArrayObject> m_CircleVAO;
		std::shared_ptr<BufferObject> m_CircleVBO;
		std::shared_ptr<Shader> m_CircleShader;
		std::vector<std::shared_ptr<RenderCommand>> m_CircleRenderCmds;
		std::vector<std::shared_ptr<RenderCommand>>::iterator m_CircleActiveRenderCmdIter;

		// For local rendering
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<Shader> m_RenderFrameBufferShader;
		std::shared_ptr<RenderCommand> m_RenderFrameBufferCmd;

		// Debug data per frame
		uint32_t m_DrawCallCnt;
		uint32_t m_RenderedCmdCnt;
	};
}