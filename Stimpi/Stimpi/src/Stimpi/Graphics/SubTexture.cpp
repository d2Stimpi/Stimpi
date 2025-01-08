#include "stpch.h"
#include "Stimpi/Graphics/SubTexture.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{
	// Note: correct asset type passed verification is done when loading asset
	SubTexture::SubTexture(AssetHandle textureAsset, glm::vec2 min, glm::vec2 max)
		: m_TextureAssetHandle(textureAsset), m_Min(min), m_Max(max)
	{
		Initialize();
	}

	SubTexture::~SubTexture()
	{
	}

	void SubTexture::Initialize()
	{
		auto texture = AssetManager::GetAsset<Texture>(m_TextureAssetHandle);
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		m_UVmin = glm::vec2(m_Min.x / textureWidth, m_Min.y / textureHeight);
		m_UVmax = glm::vec2(m_Max.x / textureWidth, m_Max.y / textureHeight);

		m_SubWidth = m_Max.x;
		m_SubHeight = m_Max.y;
	}

	void SubTexture::SetSubTextureSize(glm::vec2 min, glm::vec2 max)
	{
		m_Min = min;
		m_Max = max;

		Initialize();
	}

	void SubTexture::SetSubRegion(glm::vec2 min, glm::vec2 max)
	{
		auto texture = AssetManager::GetAsset<Texture>(m_TextureAssetHandle);
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		m_UVmin = glm::vec2(min.x / textureWidth, min.y / textureHeight);
		m_UVmax = glm::vec2(max.x / textureWidth, max.y / textureHeight);
	}

	void SubTexture::SetSubRegion(uint32_t index)
	{
		auto texture = AssetManager::GetAsset<Texture>(m_TextureAssetHandle);
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		uint32_t columns = textureWidth / m_SubWidth;
		uint32_t rows = textureHeight / m_SubHeight;

		uint32_t indexCol = index % columns;
		uint32_t indexRow = index / columns;

		m_UVmin = glm::vec2(indexCol * m_SubWidth / textureWidth, indexRow * m_SubHeight / textureHeight);
		m_UVmax = glm::vec2((indexCol + 1) * m_SubWidth / textureWidth, (indexRow + 1) * m_SubHeight / textureHeight);
	}

	Texture* SubTexture::GetTexture()	//TODO: to be removed
	{
		return AssetManager::GetAsset<Texture>(m_TextureAssetHandle).get();
	}
}