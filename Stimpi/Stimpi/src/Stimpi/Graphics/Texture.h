#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	class ST_API Texture
	{
	public:
		virtual ~Texture();

		virtual void LoadTexture(std::string file) = 0;
		virtual void UseTexture() = 0;
		virtual void Unbind() = 0;
		virtual unsigned int GetTextureID() = 0;

		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }

		static Texture* CreateTexture(std::string file);
	protected:
		int mWidth{ 0 };
		int mHeight{ 0 };
	};
}