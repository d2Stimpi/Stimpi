#pragma once

#include "Stimpi/Graphics/RenderAPI.h"

namespace Stimpi
{
	class OpenGLRenderAPI : public RenderAPI
	{
	public:
		~OpenGLRenderAPI();

		void Clear(float r, float g, float b, float a) override;
		void UnbindTexture() override;
		void GenBuffers(uint32_t num, unsigned int* buffers) override;
		void GenVertexArrays(uint32_t num, unsigned int* buffers) override;
		void DrawArrays(DrawElementsMode mode, uint32_t first, uint32_t count) override;

		void DrawElements(DrawElementsMode mode, uint32_t count, uint32_t offset) override;
	};
}