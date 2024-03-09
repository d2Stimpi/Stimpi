#include "stpch.h"
#include "Stimpi/Graphics/SubTexture.h"

namespace Stimpi
{
	
	SubTexture::SubTexture(FilePath filePath, glm::vec2 min, glm::vec2 max)
		:  m_min(min), m_max(max)
	{
		m_TextureHandle = AssetManager::GetAsset<Texture>(filePath);
	}

	SubTexture::~SubTexture()
	{
		AssetManager::Release(m_TextureHandle);
	}

	void SubTexture::Initialize()
	{
		auto texture = AssetManager::GetAsset(m_TextureHandle).As<Texture>();
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		m_UVmin = glm::vec2(m_min.x / textureWidth, m_min.y / textureHeight);
		m_UVmax = glm::vec2(m_max.x / textureWidth, m_max.y / textureHeight);

		m_SubWidth = m_max.x;
		m_SubHeight = m_max.y;
	}

	bool SubTexture::Loaded()
	{
		if (!m_Loaded)
		{
			auto texture = AssetManager::GetAsset(m_TextureHandle).As<Texture>();
			if (texture->Loaded())
			{
				if (!m_Loaded)
					Initialize();
				m_Loaded = true;
			}
		}
		return m_Loaded;
	}

	void SubTexture::SetSubTextureSize(glm::vec2 min, glm::vec2 max)
	{
		m_min = min;
		m_max = max;
	}

	void SubTexture::SetSubRegion(glm::vec2 min, glm::vec2 max)
	{
		if (!Loaded()) return;

		auto texture = AssetManager::GetAsset(m_TextureHandle).As<Texture>();
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		m_UVmin = glm::vec2(min.x / textureWidth, min.y / textureHeight);
		m_UVmax = glm::vec2(max.x / textureWidth, max.y / textureHeight);
	}

	void SubTexture::SetSubRegion(uint32_t index)
	{
		if (!Loaded()) return;

		auto texture = AssetManager::GetAsset(m_TextureHandle).As<Texture>();
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		uint32_t columns = textureWidth / m_SubWidth;
		uint32_t rows = textureHeight / m_SubHeight;

		uint32_t indexCol = index % columns;
		uint32_t indexRow = index / columns;

		m_UVmin = glm::vec2(indexCol * m_SubWidth / textureWidth, indexRow * m_SubHeight / textureHeight);
		m_UVmax = glm::vec2((indexCol + 1) * m_SubWidth / textureWidth, (indexRow + 1) * m_SubHeight / textureHeight);
	}
}