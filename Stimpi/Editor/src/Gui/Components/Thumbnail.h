#pragma once

#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Graphics/Texture.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	enum class ThumbnailType { NONE = 0, DIRECTORY, TEXTURE, SHADER, SCENE };

	static ThumbnailType GetThumbnailTypeFromFile(FilePath filePath)
	{
		std::string extension = filePath.GetFileExtension();

		if (extension == ".png" || extension == ".PNG" || extension == ".jpg" || extension == ".JPG")
			return ThumbnailType::TEXTURE;
		if (extension == ".shader")
			return ThumbnailType::SHADER;
		if (extension == ".d2s")
			return ThumbnailType::SCENE;

		return ThumbnailType::NONE;
	}

	class Thumbnail
	{
	public:

		static bool Button(const char* name, Texture* texture, ImVec2 pos, ImVec2 size);

	private:
	};
}