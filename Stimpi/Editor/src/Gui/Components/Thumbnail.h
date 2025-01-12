#pragma once

#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Asset/Asset.h"

#include "ImGui/src/imgui.h"

#include <filesystem>

namespace Stimpi
{
	enum class ThumbnailType { NONE = 0, DIRECTORY, TEXTURE, SHADER, SCENE };

	static ThumbnailType GetThumbnailTypeFromFile(FilePath filePath)
	{
		if (std::filesystem::is_directory(filePath))
			return ThumbnailType::DIRECTORY;

		AssetType assetType = AssetUtils::GetAssetType(filePath);
		switch (assetType)
		{
		case Stimpi::AssetType::NONE:		return ThumbnailType::NONE;
		case Stimpi::AssetType::TEXTURE:	return ThumbnailType::TEXTURE;
		case Stimpi::AssetType::SHADER:		return ThumbnailType::SHADER;
		case Stimpi::AssetType::SCENE:		return ThumbnailType::SCENE;
		default: return ThumbnailType::NONE;
		}

	}

	class Thumbnail
	{
	public:

		static bool Button(const char* name, Texture* texture, ImVec2 pos, ImVec2 size);

	private:
	};
}