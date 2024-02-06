#include "stpch.h"
#include "Stimpi/Scene/Assets/AssetManager.h"

namespace Stimpi
{
	template <typename T> ST_API AssetProviderData<T> AssetProvider<T>::m_ProviderData;

	std::unordered_map<uint32_t, Asset> AssetManager::m_Assets;
	std::unordered_map<std::string, Asset> AssetManager::m_NameAssets;
}