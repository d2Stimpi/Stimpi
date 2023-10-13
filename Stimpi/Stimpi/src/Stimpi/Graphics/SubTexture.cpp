#include "stpch.h"
#include "Stimpi/Graphics/SubTexture.h"

namespace Stimpi
{
	
	SubTexture::SubTexture(Texture* texture, glm::vec2 min, glm::vec2 max)
		: m_Texture(texture)
	{
		float textureWidth = m_Texture->GetWidth();
		float textureHeight = m_Texture->GetHeight();

		m_UVmin = glm::vec2(min.x / textureWidth, min.y / textureHeight);
		m_UVmax = glm::vec2(max.x / textureWidth, max.y / textureHeight);

		m_SubWidth = max.x;
		m_SubHeight = max.y;
	}

	SubTexture::~SubTexture()
	{

	}

	void SubTexture::SetSubRegion(glm::vec2 min, glm::vec2 max)
	{
		float textureWidth = m_Texture->GetWidth();
		float textureHeight = m_Texture->GetHeight();

		m_UVmin = glm::vec2(min.x / textureWidth, min.y / textureHeight);
		m_UVmax = glm::vec2(max.x / textureWidth, max.y / textureHeight);
	}

	void SubTexture::SetSubRegion(uint32_t index)
	{
		float textureWidth = m_Texture->GetWidth();
		float textureHeight = m_Texture->GetHeight();

		uint32_t columns = textureWidth / m_SubWidth;
		uint32_t rows = textureHeight / m_SubHeight;

		uint32_t indexCol = index % columns;
		uint32_t indexRow = index / columns;

		m_UVmin = glm::vec2(indexCol * m_SubWidth / textureWidth, indexRow * m_SubHeight / textureHeight);
		m_UVmax = glm::vec2((indexCol + 1) * m_SubWidth / textureWidth, (indexRow + 1) * m_SubHeight / textureHeight);
	}

}