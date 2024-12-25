#pragma once

#include "Stimpi/Log.h"
#include "Stimpi/Core/Core.h"
#include "Stimpi/Utils/FilePath.h"

#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"

#include "Stimpi/Scene/Assets/AssetHandleB.h"

#include <filesystem>

/**
 * TODO: Add clearAll() - for scene change like stuff? and app shutdown
 */

#define ASSETMANAGER_DBG (false)

namespace Stimpi
{
	enum class AssetTypeB { NONE = 0, TEXTURE, SHADER };

	/**
	 * Wrapper around asset FilePath
	 * as<T> - static method that gets real asset form AssetMaanager and casts it to T type
	 */
	class ST_API AssetB
	{
	public:
		AssetB() = default;
		AssetB(FilePath assetPath)
			: m_AssetFilePath(assetPath)
		{}

		FilePath& GetFilePath() { return m_AssetFilePath; }
		std::string GetName() { return m_AssetFilePath.GetFileName(); }
		AssetTypeB GetType() { return m_Type; }
		AssetHandleB GetHandle() { return m_Handle; }

		template <typename T>
		T* As()
		{
			return AssetProvider<T>::GetAssetData(m_Handle);
		}

		operator bool() { return m_AssetFilePath.Exists(); }
		operator FilePath() { return m_AssetFilePath; }

	private:
		FilePath m_AssetFilePath;
		AssetTypeB m_Type = AssetTypeB::NONE;
		AssetHandleB m_Handle;

		//uint32_t m_RefCount;
		friend class AssetManagerB;
	};

	/**
	 *	AssetProvider
	 */
	template <typename T>
	struct ST_API AssetProviderData
	{
		std::vector<T*> m_Data;	// with T* we get a data pool like vector, and we can reuse the slot
		std::vector<uint32_t> m_MagicNumbers;
		std::vector<uint32_t> m_RefCounters;
		std::vector<uint32_t> m_FreeSlots;
	};

	template <typename T>
	class ST_API AssetProvider
	{
	public:
		static AssetHandleB NewAssetHandle(FilePath filePath)
		{
			AssetHandleB handle;
			uint32_t index;

			// if freeSlots vector is empty we add new item
			if (m_ProviderData.m_FreeSlots.empty())
			{
				index = m_ProviderData.m_MagicNumbers.size();
				handle.Init(index);
				// create new Data element
				m_ProviderData.m_Data.push_back(T::Create(filePath));
				m_ProviderData.m_MagicNumbers.push_back(handle.GetMagic());
				// refCnt
				m_ProviderData.m_RefCounters.push_back(1);
			}
			else
			{
				index = m_ProviderData.m_FreeSlots.back();
				handle.Init(index);
				m_ProviderData.m_FreeSlots.pop_back();
				// update magic number
				m_ProviderData.m_MagicNumbers[index] = handle.GetMagic();
				m_ProviderData.m_Data[index] = T::Create(filePath);
				// refCnt
				m_ProviderData.m_RefCounters[index] = 1;
			}
			return handle;
		}

		static void IncrementRef(uint32_t index)
		{
			m_ProviderData.m_RefCounters[index]++;
		}

		static uint32_t DecrementRef(AssetHandleB& handle)
		{
			uint32_t index = handle.GetIndex();
			if (--m_ProviderData.m_RefCounters[index] == 0)
			{
				delete m_ProviderData.m_Data[index];
				m_ProviderData.m_Data[index] = nullptr;
				ST_CORE_WARN("AssetManager - delete {}, index {}", AssetManagerB::GetAsset(handle).GetName(), index);

				m_ProviderData.m_FreeSlots.push_back(index);
			}
			return m_ProviderData.m_RefCounters[index];
		}

		static T* GetAssetData(AssetHandleB handle)
		{
			if (handle.IsValid() && handle.GetMagic() == m_ProviderData.m_MagicNumbers[handle.GetIndex()])
			{
				return m_ProviderData.m_Data[handle.GetIndex()];
			}
			else
			{
				return nullptr;
			}
		}

		static void ReleaseAll()
		{
			for (auto data : m_ProviderData.m_Data)
			{
				delete data;
			}
			m_ProviderData.m_Data.clear();
			m_ProviderData.m_FreeSlots.clear();
			m_ProviderData.m_MagicNumbers.clear();
			m_ProviderData.m_RefCounters.clear();
		}

		static AssetTypeB GetType() { return AssetTypeB::NONE; }

	private:
		static AssetProviderData<T> m_ProviderData;
	};
	//template <typename T> ST_API AssetProviderData<T> AssetProvider<T>::m_ProviderData;

	// AssetProvider specializations
	template <> AssetTypeB AssetProvider<Texture>::GetType() { return AssetTypeB::TEXTURE; }
	template <> AssetTypeB AssetProvider<Shader>::GetType() { return AssetTypeB::SHADER; }

	/**
	 *	AssetManager
	 */
	class ST_API AssetManagerB
	{
	public:

		template <typename T>
		static AssetHandleB GetAsset(FilePath filePath)
		{
			AssetHandleB handle;
			std::string name = filePath.GetFileName();

			if (m_NameAssets.find(name) != m_NameAssets.end())
			{
				AssetB asset = m_NameAssets[name];
				handle = asset.GetHandle();
				AssetProvider<T>::IncrementRef(handle.GetIndex());
			}
			else
			{
				AssetB asset;
				handle = AssetProvider<T>::NewAssetHandle(filePath);
				asset = { filePath };
				asset.m_Type = AssetProvider<T>::GetType();
				asset.m_Handle = handle;
				m_NameAssets[name] = asset;
				m_Assets[handle] = asset;
			}

			if (ASSETMANAGER_DBG) ST_CORE_INFO("DBG::AssetManager - assets count: {} - {}", m_Assets.size(), m_NameAssets.size());

			return handle;
		}

		template <typename T>
		static AssetHandleB GetAssetNoRefCount(FilePath filePath)
		{
			AssetHandleB handle;
			std::string name = filePath.GetFileName();

			if (m_NameAssets.find(name) != m_NameAssets.end())
			{
				AssetB asset = m_NameAssets[name];
				handle = asset.GetHandle();
			}
			else
			{
				AssetB asset;
				handle = AssetProvider<T>::NewAssetHandle(filePath);
				asset = { filePath };
				asset.m_Type = AssetProvider<T>::GetType();
				asset.m_Handle = handle;
				m_NameAssets[name] = asset;
				m_Assets[handle] = asset;
			}

			if (ASSETMANAGER_DBG) ST_CORE_INFO("DBG::AssetManager - assets count: {} - {}", m_Assets.size(), m_NameAssets.size());

			return handle;
		}

		template <typename T>
		static T* GetAssetData(AssetHandleB handle)
		{
			return AssetProvider<T>::GetAssetData(handle);
		}

		static AssetB GetAsset(AssetHandleB handle)
		{
			if (m_Assets.find(handle) != m_Assets.end())
			{
				return m_Assets[handle];
			}
		}

		static void Release(AssetHandleB& handle)
		{
			if (handle.IsValid())
			{
				uint32_t refCnt = 1; // in case default is entered so we don't try to remove stuff
				// check if asset exists, multiple handles can call release more times??
				AssetB asset;
				if (m_Assets.find(handle) != m_Assets.end())
				{
					asset = m_Assets[handle];
				}
				switch (asset.GetType())
				{
				case AssetTypeB::TEXTURE:
					refCnt = AssetProvider<Texture>::DecrementRef(handle);
					break;
				case AssetTypeB::SHADER:
					refCnt = AssetProvider<Shader>::DecrementRef(handle);
					break;
				case AssetTypeB::NONE:
				default:
					ST_CORE_WARN("Releaseing invalid asset attempted");
					break;
				}

				if (refCnt == 0)
				{
					// remove from name-asset list
					m_NameAssets.erase(asset.GetName());
					m_Assets.erase(handle);
					handle = {};
				}
			}
		}

		static void ReleaseAll()
		{
			m_Assets.clear();
			m_NameAssets.clear();
			AssetProvider<Texture>::ReleaseAll();
			AssetProvider<Shader>::ReleaseAll();
		}

		// Debug data
		static size_t GetLoadedAssetsCount()
		{
			return m_Assets.size();
		}

	private:
		static std::unordered_map<uint32_t, AssetB> m_Assets;
		static std::unordered_map<std::string, AssetB> m_NameAssets;
	};
}