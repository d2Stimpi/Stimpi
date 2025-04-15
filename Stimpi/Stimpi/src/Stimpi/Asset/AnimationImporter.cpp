#include "stpch.h"
#include "Stimpi/Asset/AnimationImporter.h"

#include "Stimpi/Core/Project.h"

namespace Stimpi
{

	std::shared_ptr<Asset> AnimationImporter::ImportAnimation(AssetHandle handle, const AssetMetadata& metadata)
	{
		FilePath assetPath = Project::GetAssestsDir() / metadata.m_FilePath.string();
		std::shared_ptr<Animation> animation = LoadAnimation(assetPath);
		animation->m_Handle = handle;
		return std::static_pointer_cast<Asset>(animation);
	}

	std::shared_ptr<Animation> AnimationImporter::LoadAnimation(const FilePath& filePath)
	{
		std::shared_ptr<Animation> animation = Animation::Create(filePath);
		return animation;
	}

}