#include "stpch.h"
#include "Stimpi/Asset/SceneImporter.h"

#include "Stimpi/Scene/Scene.h"

namespace Stimpi
{

	std::shared_ptr<Stimpi::Asset> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
	{
		auto sceneAsset = std::make_shared<Scene>();
		sceneAsset->SetName(metadata.m_FilePath.GetFileName());
		return std::static_pointer_cast<Asset>(sceneAsset);
	}

}