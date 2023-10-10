#include "stpch.h"
#include "Stimpi/Graphics/Texture.h"

#include "Stimpi/Log.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLTexture.h"

namespace Stimpi
{
	Texture::~Texture()
	{

	}

	Texture* Texture::CreateTexture(std::string file)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLTexture(file);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}

		ST_CORE_CRITICAL("Unknown GraphicsAPI");
		return nullptr;
	}

	Texture* Texture::CreateFrameBufferTexture()
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLTexture();
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}

		ST_CORE_CRITICAL("Unknown GraphicsAPI");
		return nullptr;
	}
}