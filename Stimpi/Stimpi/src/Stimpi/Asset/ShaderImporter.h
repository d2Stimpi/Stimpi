#pragma once

#include "Stimpi/Asset/AssetMetadata.h"
#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{
	class ShaderImporter
	{
	public:
		static std::shared_ptr<Asset> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Shader> LoadShader(const FilePath& filePath);
	};
}