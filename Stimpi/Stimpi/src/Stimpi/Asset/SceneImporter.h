#pragma once

#include "Stimpi/Asset/AssetMetadata.h"

namespace Stimpi
{
	class SceneImporter
	{
	public:
		static std::shared_ptr<Asset> ImportScene(AssetHandle handle, const AssetMetadata& metadata);
	};
}