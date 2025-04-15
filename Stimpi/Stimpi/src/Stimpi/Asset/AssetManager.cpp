#include "stpch.h"
#include "Stimpi/Asset/AssetManager.h"

#include "Stimpi/Core/Project.h"

namespace Stimpi
{

	Stimpi::AssetHandle AssetManager::GetAssetHandle(const AssetMetadata& metadata)
	{
		return Project::GetAssetManager()->GetAssetHandle(metadata);
	}

}