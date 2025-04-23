#include "stpch.h"
#include "Gui/Prefab/PrefabManager.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	void PrefabManager::OnPrefabAssetReload(std::shared_ptr<Asset> asset)
	{
		if (asset->GetType() == AssetType::PREFAB)
		{
			// Update prefab instances
		}

		ST_INFO("PrefabManager: asset reloaded {}", asset->m_Handle);
	}

}