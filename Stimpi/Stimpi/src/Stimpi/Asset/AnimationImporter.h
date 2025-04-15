#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetMetadata.h"
#include "Stimpi/Graphics/Animation/Animation.h"

namespace Stimpi
{
	class AnimationImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAnimation(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Animation> LoadAnimation(const FilePath& filePath);
	};
}