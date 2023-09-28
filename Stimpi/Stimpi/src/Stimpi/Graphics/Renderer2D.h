#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/RenderAPI.h"
#include "Stimpi/Graphics/RenderCommand.h"

//TODO: combine in one header
#include "Stimpi/Graphics/BufferObject.h"
#include "Stimpi/Graphics/Camera.h"
#include "Stimpi/Graphics/FrameBuffer.h"
#include "Stimpi/Graphics/Shader.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/VertexArrayObject.h"

#define VERTEX_ARRAY_SIZE	4096
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

		void BeginScene(Camera* camera, Shader* shader);
		void EndScene();
		void PushQuad(float x, float y, float width, float height, float u = 1.0f, float v = 1.0f);
		
		// TODO: add various Submitt methods
		void Submit(glm::vec4 quad);
		void Submit(glm::vec4 quad, Texture* texture);
		// TODO: Submit(quad, color)
		// TODO: Submit(quad, texture)

		void UseTexture(Texture* texture);
		void RenderTarget(FrameBuffer* target); // TODO: needed?

		// Event Callbacks
		void ResizeCanvas(uint32_t width, uint32_t height);

		// Canvas - FB size
		uint32_t GetCanvasWidth() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetWidth(); };
		uint32_t GetCanvasHeight() { if (m_FrameBuffer != nullptr) return m_FrameBuffer->GetHeight(); };

		//Internal - by frame
		void StartFrame();
		void DrawFrame();
		void EndFrame();

		//To link with ImGui
		FrameBuffer* GetFrameBuffer();

	private:
		void DrawRenderCmd(std::shared_ptr<RenderCommand>& renderCmd);
		void ShowDebugData();

		RenderAPI* m_RenderAPI;
		std::vector<std::shared_ptr<RenderCommand>> m_RenderCmds;
		std::vector<std::shared_ptr<RenderCommand>>::iterator m_ActiveRenderCmdIter;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<VertexArrayObject> m_VAO;
		std::shared_ptr<BufferObject> m_VBO;

		//Dbg data per frame
		uint32_t m_DrawCallCnt;
		uint32_t m_RenderedCmdCnt;
	};
}