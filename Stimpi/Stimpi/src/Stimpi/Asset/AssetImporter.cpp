#include "stpch.h"
#include "Stimpi/Asset/AssetImporter.h"

#include "Stimpi/Log.h"
#include "Stimpi/Asset/TextureImporter.h"
#include "Stimpi/Asset/SceneImporter.h"
#include "Stimpi/Asset/ShaderImporter.h"
#include "Stimpi/Asset/AnimationImporter.h"
#include "Stimpi/Utils/ThreadPool.h"

#include <chrono>

namespace Stimpi
{
	using namespace std::chrono_literals;
	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata&)>;

	std::unordered_map<AssetHandle, std::future<std::shared_ptr<Asset>>> s_LoadingFutures;
	std::unordered_map<AssetType, AssetImportFunction> s_AssetImporters =
	{
		{ AssetType::SHADER, ShaderImporter::ImportShader },
		{ AssetType::SCENE, SceneImporter::ImportScene },
		{ AssetType::TEXTURE, TextureImporter::ImportTexture },
		{ AssetType::ANIMATION, AnimationImporter::ImportAnimation }
	};

	std::shared_ptr<Stimpi::Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImporters.find(metadata.m_Type) == s_AssetImporters.end())
		{
			ST_CORE_ERROR("AssetImporter::ImportAsset: Unknown asset type!");
			return nullptr;
		}

		return s_AssetImporters.at(metadata.m_Type)(handle, metadata);
	}

	// TODO: find most appropriate use case, ex. when loading a whole scene
	std::shared_ptr<Asset> AssetImporter::ImportAssetAsync(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImporters.find(metadata.m_Type) == s_AssetImporters.end())
		{
			ST_CORE_ERROR("AssetImporter::ImportAsset: Unknown asset type!");
			return nullptr;
		}

		FilePath path = metadata.m_FilePath;
		std::shared_ptr<Asset> asset = nullptr;
		// Check if we are waiting for asset loading future
		if (s_LoadingFutures.find(handle) != s_LoadingFutures.end())
		{
			auto& future = s_LoadingFutures.at(handle);
			if (future.valid())
			{
				if (future.wait_for(1ns) == std::future_status::ready)
				{
					asset = future.get();
					if (!asset)
					{
						ST_CORE_ERROR("Failed to import asset: {}", path.string());
					}

					// clear the registered future
					s_LoadingFutures.erase(handle);
					ST_CORE_INFO("Finished loading Asset [{}]", path.string());
				}
			}
		}
		else
		{
			s_LoadingFutures[handle] = s_LoaderThreadPool->AddTask([&]()
				{
					return s_AssetImporters.at(metadata.m_Type)(handle, metadata);
				});
			ST_CORE_INFO("Started loading Asset [{}]", path.string());
		}

		return asset;
	}

}