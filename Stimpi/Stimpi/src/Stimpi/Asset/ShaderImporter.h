#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class ShaderImporter
	{
	public:
		static std::shared_ptr<Asset> ImportShader(AssetHandle handle, const AssetMetadata& metadata);
	};
}