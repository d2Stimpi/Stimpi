#include "stpch.h"
#include "Stimpi/Graphics/Animation/AnimatedSprite.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	AnimatedSprite::AnimatedSprite(FilePath path)
		: m_Animation(std::make_shared<Animation>())
	{
		m_Animation.reset(Animation::Create(path));
		m_FramesCount = m_Animation->GetFrames().size();
	}

	AnimatedSprite::AnimatedSprite()
		: m_Animation(std::make_shared<Animation>())
	{}

	void AnimatedSprite::SetAnimation(FilePath path)
	{
		m_Animation.reset(Animation::Create(path));
		m_FramesCount = m_Animation->GetFrames().size();
	}

	void AnimatedSprite::SetAnimation(std::shared_ptr<Animation> animation)
	{
		m_Animation = animation;
		m_FramesCount = m_Animation->GetFrames().size();
		// Reset anim if any active
		SetCurrentFrame(0);
	}

	void AnimatedSprite::Update(Timestep ts)
	{
		if (!m_Animation || m_Animation->GetFrames().empty())
			return;

		if (m_State == AnimationState::RUNNING)
		{
			m_TimeElapsed += ts;
			m_FrameDuration = m_Animation->GetFrames().at(m_CurrentFrame).m_Duration;
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
					m_CurrentFrame = m_Animation->GetFrames().size() - 1;
			}

			//ST_CORE_INFO("Elapsed {}, FrameTime {}", m_TimeElapsed, m_FrameTime);
		}

		// Cap the index before access
		if (m_CurrentFrame >= m_Animation->GetFrames().size())
		{
			m_CurrentFrame = m_Animation->GetFrames().size() - 1;
			ST_CORE_WARN("[AnimatedSprite] Current frame exceeded frame count ({})", m_Animation->GetName());
		}

		auto& currentFrame = m_Animation->GetFrames().at(m_CurrentFrame);
		m_Animation->GetSubTexture()->SetSubTextureSize({ 0.0f, 0.0f }, { currentFrame.m_FrameSize.x, currentFrame.m_FrameSize.y });
		//m_Animation->GetSubTexture()->SetSubRegion(m_CurrentFrame);
		m_Animation->GetSubTexture()->SetSubRegion({ currentFrame.m_FramePosition.x, currentFrame.m_FramePosition.y },
			{ currentFrame.m_FramePosition.x + currentFrame.m_FrameSize.x, currentFrame.m_FramePosition.y + currentFrame.m_FrameSize.y } );
	}

	void AnimatedSprite::SetCurrentFrame(uint32_t frame)
	{
		m_CurrentFrame = frame;
		auto& currentFrame = m_Animation->GetFrames().at(m_CurrentFrame);
		m_Animation->GetSubTexture()->SetSubTextureSize({ 0.0f, 0.0f }, { currentFrame.m_FrameSize.x, currentFrame.m_FrameSize.y });
		//m_Animation->GetSubTexture()->SetSubRegion(m_CurrentFrame);
		m_Animation->GetSubTexture()->SetSubRegion({ currentFrame.m_FramePosition.x, currentFrame.m_FramePosition.y },
			{ currentFrame.m_FramePosition.x + currentFrame.m_FrameSize.x, currentFrame.m_FramePosition.y + currentFrame.m_FrameSize.y });
	}

}