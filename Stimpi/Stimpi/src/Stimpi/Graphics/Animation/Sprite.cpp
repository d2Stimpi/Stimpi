#include "stpch.h"
#include "Stimpi/Graphics/Animation/Sprite.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	Sprite::Sprite(SubTexture* texture, uint32_t frames, float duration)
		: m_FramesCount(frames), m_AnimDuration(duration), m_SpriteTexture(texture)
	{
		m_FrameTime = m_AnimDuration / m_FramesCount;
	}

	Sprite::~Sprite()
	{

	}

	void Sprite::SetCurrentFrame(uint32_t frame)
	{
		//if (frame <= m_FramesCount && frame >= 0)
		{
			m_CurrentFrame = frame;
			m_SpriteTexture->SetSubRegion(m_CurrentFrame);
		}
	}

	void Sprite::Update(Timestep ts)
	{
		if (m_AnimState == SpriteState::RUNNING)
		{
			m_TimeElapsed += ts;

			if (m_TimeElapsed >= m_FrameTime)
			{
				m_CurrentFrame++;
				m_CurrentFrame %= m_FramesCount;

				// Reset time track, and keep the "change"
				m_TimeElapsed -= m_FrameTime;
				if ((m_CurrentFrame == m_FramesCount - 1) && (!m_Looping))
				{
					m_AnimState = SpriteState::COMPELETED;
				}
			}
		}

		m_SpriteTexture->SetSubRegion(m_CurrentFrame);
	}

}