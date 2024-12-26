#include "stpch.h"
#include "Stimpi/Graphics/Texture.h"

#include "Stimpi/Log.h"
#include "Stimpi/Graphics/Graphics.h"
#include "Stimpi/Platform/OpenGLTexture.h"


namespace Stimpi
{
	Texture::~Texture()
	{
		//ST_CORE_INFO("~Texture");
	}

	Texture* Texture::CreateTexture(std::string filePath)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return new OpenGLTexture(filePath);
		case GraphicsAPI::None: ST_CORE_CRITICAL("GraphicsAPI: not supported!"); return nullptr;
		}

		ST_CORE_CRITICAL("Unknown GraphicsAPI");
		return nullptr;
	}

	std::shared_ptr<Texture> Texture::CreateTexture(TextureSpecification spec)
	{
		switch (Graphics::GetAPI())
		{
		case GraphicsAPI::OpenGL: return std::make_shared<OpenGLTexture>(spec);
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