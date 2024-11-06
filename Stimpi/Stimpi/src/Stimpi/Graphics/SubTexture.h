#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Texture.h"
#include "Stimpi/Utils/FilePath.h"
#include "Stimpi/Scene/Assets/AssetHandle.h"

#include "stpch.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	class ST_API SubTexture
	{
	public:
		SubTexture(FilePath filePath, glm::vec2 min, glm::vec2 max);
		~SubTexture();

		void Initialize();
		bool Loaded();

		void SetSubTextureSize(glm::vec2 min, glm::vec2 max);

		void SetSubRegion(glm::vec2 min, glm::vec2 max);
		void SetSubRegion(uint32_t index);

		uint32_t GetSubWidht() { return m_SubWidth; }
		uint32_t GetSubHeight() { return m_SubHeight; }

		glm::vec2 GetUVMin() { return m_UVmin; }
		glm::vec2 GetUVMax() { return m_UVmax; }

		Texture* GetTexture();
		unsigned int GetTextureID() { return GetTexture()->GetTextureID(); }

		AssetHandle GetAssetHandle() { return m_TextureHandle; }

	private:
		bool m_Loaded = false;
		AssetHandle m_TextureHandle = {};
		glm::vec2 m_Min = { 0.0f, 0.0f };
		glm::vec2 m_Max = { 0.0f, 0.0f };
		glm::vec2 m_UVmin = { 0.0f, 0.0f };
		glm::vec2 m_UVmax = { 0.0f, 0.0f };
		uint32_t m_SubWidth = 0;
		uint32_t m_SubHeight = 0;
	};
}