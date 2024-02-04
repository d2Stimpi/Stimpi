#include "stpch.h"
#include "Stimpi/Scene/Assets/AssetManager.h"

namespace Stimpi
{
	std::unordered_map<uint32_t, Asset> AssetManager::m_Assets;
	std::unordered_map<std::string, Asset> AssetManager::m_NameAssets;
}