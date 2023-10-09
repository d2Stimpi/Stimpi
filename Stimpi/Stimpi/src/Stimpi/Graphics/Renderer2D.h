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
#include "Stimpi/Graphics/VertexArrayObject.h"

#define VERTEX_CMD_CAPACITY (6 * 10000) // 6 vertex per quad, 10k quads
#define VERTEX_ARRAY_SIZE	(192 * 1000) // Layout{3,3,2} quad szie * 1000 (max 1k squares per call)  
#define RENDERER_DBG	(true)

/* TODO:
* - Finish configuring of non-ui (editor) rendering. Rendering of FrameBuffer
*/

namespace Stimpi
{
	class ST_API Renderer2D
	{
	public:
		Renderer2D();
		~Renderer2D();

		static Renderer2D* Instace();

		void BeginScene(OrthoCamera* camera);
		void EndScene();
		
		// New
		void Flush();
		void Submit(glm::vec4 quad, Texture* texture, Shader* shader);
		void Submit(glm::vec4 quad, Shader* shader);
		void Submit(glm::vec4 quad, glm::vec3 color, Shader* shader);

		void RenderTarget(FrameBuffer* target); // TODO: needed?

		// Event Callbacks
		void ResizeCanvas(uint32_t width, uint32_t height);

		// Canvas - FB size
		uint32_t GetCanvasWidth() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetWidth(); };
		uint32_t GetCanvasHeight() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetHeight(); };

		//Render queued stuff
		void StartFrame();
		void DrawFrame();
		void EndFrame();

		//To link with ImGui
		FrameBuffer* GetFrameBuffer();

	private:
		void PushQuadVertexData(RenderCommand* cmd, glm::vec4 quad, glm::vec3 color = { 1.0f, 1.0f, 1.0f }, glm::vec2 min = { 0.0f, 0.0f }, glm::vec2 max = { 1.0f, 1.0f });
		void DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);

		void CheckCapacity();
		void CheckTextureBatching(Texture* texture);

		void ShowDebugData();
	private:
		RenderAPI* m_RenderAPI;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<VertexArrayObject> m_VAO;
		std::shared_ptr<BufferObject> m_VBO;

		OrthoCamera* m_ActiveCamera;
		std::vector<std::shared_ptr<RenderCommand>> m_NewRenderCmds;
		std::vector<std::shared_ptr<RenderCommand>>::iterator m_ActiveNewRenderCmdIter;

		//Dbg data per frame
		uint32_t m_DrawCallCnt;
		uint32_t m_RenderedCmdCnt;
	};
}