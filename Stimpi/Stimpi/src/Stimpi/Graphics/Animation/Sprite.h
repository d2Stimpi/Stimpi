#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/SubTexture.h"
#include "Stimpi/Core/Timestep.h"

#include <cstdint>

/* TODO: Non-uniform frame duration */

namespace Stimpi
{
	enum class SpriteState {RUNNING = 0, PAUSED, STOPPED, COMPELETED};	// Stopped - start form 0 frame; COMPELETED - used for maintaning correct UI data

	class ST_API Sprite
	{
	public:
		Sprite(SubTexture* texture, uint32_t frames, float duration);
		~Sprite();

		void SetAnimatable(bool animatable) { m_Animatable = animatable; }
		void SetLooping(bool looping) { m_Looping = looping; }
		void SetDuration(float duration) { m_AnimDuration = duration; m_FrameTime = m_AnimDuration / m_FramesCount; }
		void SetFramesCount(uint32_t framesCount) { m_FramesCount = framesCount; }
		void SetCurrentFrame(uint32_t frame);

		bool GetAnimatable() { return m_Animatable; }
		bool GetLooping() { return m_Looping; }
		float GetDuration() { return m_AnimDuration; }
		uint32_t GetFramesCount() { return m_FramesCount; }
		uint32_t GetCurrentFrame() { return m_CurrentFrame; }

		SpriteState GetSpriteState() { return m_AnimState; }

		void Stop() { m_AnimState = SpriteState::STOPPED; m_CurrentFrame = 0; m_TimeElapsed = 0.0f; }
		void Start() { m_AnimState = SpriteState::RUNNING; }
		void Pause() { m_AnimState = SpriteState::PAUSED;  }

		void Update(Timestep ts);

	private:
		bool m_Animatable = true;
		bool m_Looping = false;
		uint32_t m_CurrentFrame = 0;
		uint32_t m_FramesCount;
		float m_AnimDuration;
		SpriteState m_AnimState = SpriteState::STOPPED;

		SubTexture* m_SpriteTexture;

		// Internals
		float m_FrameTime = 0.0f;
		float m_TimeElapsed = 0.0;
	};
}