#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Texture.h"

#include "stpch.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	class ST_API SubTexture
	{
	public:
		SubTexture(Texture* texture, glm::vec2 min, glm::vec2 max);
		~SubTexture();

		void SetSubRegion(glm::vec2 min, glm::vec2 max);
		void SetSubRegion(uint32_t index);

		uint32_t GetSubWidht() { return m_SubWidth; }
		uint32_t GetSubHeight() { return m_SubHeight; }

		glm::vec2 GetUVMin() { return m_UVmin; }
		glm::vec2 GetUVMax() { return m_UVmax; }

		Texture* GetTexture() { return m_Texture; }
		unsigned int GetTextureID() { return m_Texture->GetTextureID(); }

	private:
		Texture* m_Texture;
		glm::vec2 m_UVmin;
		glm::vec2 m_UVmax;
		uint32_t m_SubWidth;
		uint32_t m_SubHeight;
	};
}