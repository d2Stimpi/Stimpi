#include "stpch.h"
#include "Stimpi/Graphics/SubTexture.h"

namespace Stimpi
{
	
	SubTexture::SubTexture(Texture* texture, glm::vec2 min, glm::vec2 max)
		: m_Texture(texture)
	{
		float textureWidth = texture->GetWidth();
		float textureHeight = texture->GetHeight();

		m_UVmin = glm::vec2(min.x / textureWidth, min.y / textureHeight);
		m_UVmax = glm::vec2(max.x / textureWidth, max.y / textureHeight);
	}

	SubTexture::~SubTexture()
	{

	}

}