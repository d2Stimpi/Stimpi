#include "stpch.h"
#include "Stimpi/Scene/Assets/AssetManagerB.h"

namespace Stimpi
{
	template <typename T> ST_API AssetProviderData<T> AssetProvider<T>::m_ProviderData;

	std::unordered_map<uint32_t, AssetB> AssetManagerB::m_Assets;
	std::unordered_map<std::string, AssetB> AssetManagerB::m_NameAssets;
}