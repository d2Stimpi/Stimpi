#include "stpch.h"
#include "Stimpi/Asset/SceneImporter.h"

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		return std::make_shared<Scene>();
	}

}