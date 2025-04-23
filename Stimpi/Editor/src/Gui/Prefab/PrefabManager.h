#pragma once

#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Asset/Prefab.h"

namespace Stimpi
{
	class PrefabManager
	{
	public:
		static void OnPrefabAssetReload(std::shared_ptr<Asset> asset);
	};
}