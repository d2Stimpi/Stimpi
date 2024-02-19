#pragma once

#include "Stimpi/Log.h"
#include "Stimpi/Core/Core.h"
#include "Stimpi/Utils/FilePath.h"

#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Graphics/Shader.h"

#include <filesystem>

/**
 * TODO: Add clearAll() - for scene change like stuff? and app shutdown
 */

#define ASSETMANAGER_DBG (false)

namespace Stimpi
{
	enum class AssetType { NONE = 0, TEXTURE, SHADER };

	/**
	 *	AssetHandle
	 */
	class ST_API AssetHandle
	{
	public:
		AssetHandle() : m_Handle(0) {}

		void Init(uint32_t index)
		{
			assert(m_Handle == 0);
			assert(index <= MAX_INDEX);

			static uint32_t s_MagiCouter = 0;
			if (++s_MagiCouter > MAX_MAGIC)
			{
				s_MagiCouter = 1;
			}

			m_Index = index;
			m_Magic = s_MagiCouter;

		}

		uint32_t GetIndex() { return m_Index; }
		uint32_t GetMagic() { return m_Magic; }
		uint32_t GetHandle() { return m_Handle; }
		bool IsValid() { return m_Handle != 0; }

		operator uint32_t() { return m_Handle; }

	private:
		union
		{
			enum
			{
				INDEX_BITS = 16,
				MAGIC_BITS = 16,

				MAX_INDEX = (1 << INDEX_BITS) - 1,
				MAX_MAGIC = (1 << MAGIC_BITS) - 1
			};

			struct
			{
				unsigned m_Index : INDEX_BITS;
				unsigned m_Magic : MAGIC_BITS;
			};
			uint32_t m_Handle;
		};
	};

	/**
	 * Wrapper around asset FilePath
	 * as<T> - static method that gets real asset form AssetMaanager and casts it to T type
	 */
	class ST_API Asset
	{
	public:
		Asset() = default;
		Asset(FilePath assetPath)
			: m_AssetFilePath(assetPath)
		{}

		FilePath& GetFilePath() { return m_AssetFilePath; }
		std::string GetName() { return m_AssetFilePath.GetFileName(); }
		AssetType GetType() { return m_Type; }
		AssetHandle GetHandle() { return m_Handle; }

		template <typename T>
		T* As()
		{
			return AssetProvider<T>::GetAssetData(m_Handle);
		}

		operator bool() { return m_AssetFilePath.Exists(); }
		operator FilePath() { return m_AssetFilePath; }

	private:
		FilePath m_AssetFilePath;
		AssetType m_Type = AssetType::NONE;
		AssetHandle m_Handle;

		//uint32_t m_RefCount;
		friend class AssetManager;
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
		static AssetHandle NewAssetHandle(FilePath filePath)
		{
			AssetHandle handle;
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

		static uint32_t DecrementRef(AssetHandle& handle)
		{
			uint32_t index = handle.GetIndex();
			if (--m_ProviderData.m_RefCounters[index] == 0)
			{
				delete m_ProviderData.m_Data[index];
				m_ProviderData.m_Data[index] = nullptr;
				ST_CORE_WARN("AssetManager - delete {}, index {}", AssetManager::GetAsset(handle).GetName(), index);

				m_ProviderData.m_FreeSlots.push_back(index);
			}
			return m_ProviderData.m_RefCounters[index];
		}

		static T* GetAssetData(AssetHandle handle)
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

		static AssetType GetType() { return AssetType::NONE; }

	private:
		static AssetProviderData<T> m_ProviderData;
	};
	//template <typename T> ST_API AssetProviderData<T> AssetProvider<T>::m_ProviderData;

	// AssetProvider specializations
	template <> AssetType AssetProvider<Texture>::GetType() { return AssetType::TEXTURE; }
	template <> AssetType AssetProvider<Shader>::GetType() { return AssetType::SHADER; }

	/**
	 *	AssetManager
	 */
	class ST_API AssetManager
	{
	public:

		template <typename T>
		static AssetHandle GetAsset(FilePath filePath)
		{
			AssetHandle handle;
			std::string name = filePath.GetFileName();

			if (m_NameAssets.find(name) != m_NameAssets.end())
			{
				Asset asset = m_NameAssets[name];
				handle = asset.GetHandle();
				AssetProvider<T>::IncrementRef(handle.GetIndex());
			}
			else
			{
				Asset asset;
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
		static AssetHandle GetAssetNoRefCount(FilePath filePath)
		{
			AssetHandle handle;
			std::string name = filePath.GetFileName();

			if (m_NameAssets.find(name) != m_NameAssets.end())
			{
				Asset asset = m_NameAssets[name];
				handle = asset.GetHandle();
			}
			else
			{
				Asset asset;
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
		static T* GetAssetData(AssetHandle handle)
		{
			return AssetProvider<T>::GetAssetData(handle);
		}

		static Asset GetAsset(AssetHandle handle)
		{
			if (m_Assets.find(handle) != m_Assets.end())
			{
				return m_Assets[handle];
			}
		}

		static void Release(AssetHandle& handle)
		{
			if (handle.IsValid())
			{
				uint32_t refCnt = 1; // in case default is entered so we don't try to remove stuff
				// check if asset exists, multiple handles can call release more times??
				Asset asset;
				if (m_Assets.find(handle) != m_Assets.end())
				{
					asset = m_Assets[handle];
				}
				switch (asset.GetType())
				{
				case AssetType::TEXTURE:
					refCnt = AssetProvider<Texture>::DecrementRef(handle);
					break;
				case AssetType::SHADER:
					refCnt = AssetProvider<Shader>::DecrementRef(handle);
					break;
				case AssetType::NONE:
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
		static uint32_t GetLoadedAssetsCount()
		{
			return m_Assets.size();
		}

	private:
		static std::unordered_map<uint32_t, Asset> m_Assets;
		static std::unordered_map<std::string, Asset> m_NameAssets;
	};
}