#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Asset/AssetMetadata.h"
#include "Stimpi/Asset/Prefab.h"

namespace Stimpi
{
	class ST_API PrefabImporter
	{
	public:
		static std::shared_ptr<Asset> ImportPrefab(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Prefab> LoadPrefab(const FilePath& filePath);

	};
}