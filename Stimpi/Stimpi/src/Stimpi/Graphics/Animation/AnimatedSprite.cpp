#include "stpch.h"
#include "Stimpi/Graphics/Animation/AnimatedSprite.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Asset/AssetManager.h"

namespace Stimpi
{

	AnimatedSprite::AnimatedSprite(AssetHandle animationHandle)
		: m_AnimationHandle(animationHandle)
	{
		m_SubTexture = std::make_shared<SubTexture>(0, glm::vec2{ 0.0f, 0.0f }, glm::vec2{ 0.0f, 0.0f });
		if (animationHandle)
		{
			std::shared_ptr<Animation> animation = GetAnimation();
			m_FramesCount = animation->GetFrames().size();
			m_SubTexture->SetTextureHandle(animation->GetTexture());
		}
	}

	AnimatedSprite::AnimatedSprite()
		: AnimatedSprite(0)
	{}

	void AnimatedSprite::SetAnimation(AssetHandle animationHandle)
	{
		m_AnimationHandle = animationHandle;
		std::shared_ptr<Animation> animation = GetAnimation();
		if (animation)
		{
			m_FramesCount = animation->GetFrames().size();
			m_SubTexture->SetTextureHandle(animation->GetTexture());
		}
		else
			m_FramesCount = 0;

		SetCurrentFrame(0);
		m_TimeElapsed = 0.0f;
	}

	std::shared_ptr<Animation> AnimatedSprite::GetAnimation()
	{
		if (m_AnimationHandle)
			return AssetManager::GetAsset<Animation>(m_AnimationHandle);
		else
			return nullptr;
	}

	void AnimatedSprite::Update(Timestep ts)
	{
		std::shared_ptr<Animation> animation = GetAnimation();

		if (!animation || animation->GetFrames().empty())
			return;

		if (m_State == AnimationState::RUNNING)
		{
			m_TimeElapsed += ts;
			m_FrameDuration = animation->GetFrames().at(m_CurrentFrame).m_Duration;
			// Apply playback speed factor
			m_FrameDuration = m_FrameDuration / m_PlaybackSpeed;

			if (m_TimeElapsed >= m_FrameDuration)
			{
				m_CurrentFrame++;

				// Reset time track, and keep the "change"
				m_TimeElapsed -= m_FrameDuration;
				if ((m_CurrentFrame == m_FramesCount - 1) && m_WrapMode == AnimationWrapMode::ONCE)
				{
					m_State = AnimationState::COMPELETED;
				}

				if (m_WrapMode == AnimationWrapMode::LOOPING)
					m_CurrentFrame %= m_FramesCount - 1;
				else if (m_WrapMode == AnimationWrapMode::CLAMP)
					m_CurrentFrame = animation->GetFrames().size() - 1;
			}

			//ST_CORE_INFO("Elapsed {}, FrameTime {}", m_TimeElapsed, m_FrameTime);
		}

		// Cap the index before access
		if (m_CurrentFrame >= animation->GetFrames().size())
		{
			m_CurrentFrame = animation->GetFrames().size() - 1;
			ST_CORE_WARN("[AnimatedSprite] Current frame exceeded frame count ({})", animation->GetName());
		}

		SetCurrentFrame(m_CurrentFrame);
	}

	void AnimatedSprite::SetCurrentFrame(uint32_t frame)
	{
		std::shared_ptr<Animation> animation = GetAnimation();
		if (animation)
		{
			m_CurrentFrame = frame;
			auto& currentFrame = animation->GetFrames().at(m_CurrentFrame);
			m_SubTexture->SetSubTextureSize({ 0.0f, 0.0f }, { currentFrame.m_FrameSize.x, currentFrame.m_FrameSize.y });
			m_SubTexture->SetSubRegion({ currentFrame.m_FramePosition.x, currentFrame.m_FramePosition.y },
				{ currentFrame.m_FramePosition.x + currentFrame.m_FrameSize.x, currentFrame.m_FramePosition.y + currentFrame.m_FrameSize.y });
		}
	}

	Stimpi::SubTexture* AnimatedSprite::GetSubTexture()
	{
		return m_SubTexture.get();
	}

}