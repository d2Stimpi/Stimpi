#pragma once

namespace Stimpi
{
	// TODO: add here to support more DrawElements modes
	enum class DrawElementsMode { TRIANGLES = 0, LINES };

	class RenderAPI
	{
	public:
		virtual ~RenderAPI();

		// Used at start/end of the frame
		virtual void EnableBlend() = 0;
		virtual void Clear(float r, float g, float b, float a) = 0;
		virtual void UnbindTexture() = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void GenBuffers(uint32_t num, unsigned int* buffers) = 0;
		virtual void GenVertexArrays(uint32_t num, unsigned int* buffers) = 0;

		virtual void DrawElements(DrawElementsMode mode, uint32_t count, uint32_t offset) = 0;
		virtual void DrawArrays(DrawElementsMode mode, uint32_t first, uint32_t count) = 0;

		virtual void SetLineWidth(float width) = 0;

		static RenderAPI* CreateRenderAPI();
	private:
	};
}