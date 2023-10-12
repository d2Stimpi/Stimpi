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

		glm::vec2 GetUVMin() { return m_UVmin; }
		glm::vec2 GetUVMax() { return m_UVmax; }
		Texture* GetTexture() { return m_Texture; }

	private:
		Texture* m_Texture;
		glm::vec2 m_UVmin;
		glm::vec2 m_UVmax;
	};
}