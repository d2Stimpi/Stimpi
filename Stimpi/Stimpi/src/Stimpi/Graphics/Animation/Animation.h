#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/SubTexture.h"
#include "Stimpi/Asset/Asset.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	struct ST_API AnimationFrameData
	{
		float m_Duration = 0;	// in seconds
		glm::vec2 m_FramePosition = { 0.0f, 0.0f };
		glm::vec2 m_FrameSize = { 0.0f, 0.0f };	// TODO: atm not used in any way

		AnimationFrameData() = default;
		AnimationFrameData(const AnimationFrameData&) = default;
		AnimationFrameData(uint32_t duration, glm::vec2 pos, glm::vec2 size)
			: m_Duration(duration), m_FramePosition(pos), m_FrameSize(size)
		{}
	};

	class ST_API Animation : public Asset
	{
	public:
		Animation();
		~Animation();

		AssetHandle GetTexture();
		std::vector<AnimationFrameData>& GetFrames() { return m_Frames; }
		FilePath GetAssetFilePath() { return m_AssetPath; }
		std::string& GetName() { return m_Name; }

		// AssetManager resource
		static std::shared_ptr<Animation> Create(std::string file);

		static AssetType GetTypeStatic() { return AssetType::ANIMATION; }
		AssetType GetType() override { return GetTypeStatic(); }

	private:
		FilePath m_AssetPath;
		std::string m_Name;
		AssetHandle m_TextureHandle;
		std::vector<AnimationFrameData> m_Frames;

		friend class AnimationSerializer;
	};
}