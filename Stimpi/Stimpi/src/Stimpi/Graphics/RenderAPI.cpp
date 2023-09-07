#include "Stimpi/Graphics/RenderAPI.h"

#include "Stimpi/Platform/OpenGLRenderAPI.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Log.h"

namespace Stimpi
{
	RenderAPI::~RenderAPI()
	{

	}

	RenderAPI* RenderAPI::CreateRenderAPI()
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLRenderAPI();
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}
	}
}