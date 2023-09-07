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
		void PushQuad(float x, float y, float width, float height, float u, float v); // x,y,w,h,u,v
		void UseTexture(Texture* texture);

		//Internal - by frame
		void StartFrame();
		void DrawFrame();
		void EndFrame();

		//To link with ImGui
		FrameBuffer* GetFrameBuffer();

	private:
		RenderAPI* m_RenderAPI;
		std::vector<std::shared_ptr<RenderCommand>> m_RenderCmds;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		std::shared_ptr<VertexArrayObject> m_VAO;
		std::shared_ptr<BufferObject> m_VBO;

		//Active scene tracking data
		std::shared_ptr<RenderCommand> m_ActiveCmd;
	};
}