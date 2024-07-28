#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Graphics/Animation/Animation.h"
#include "Stimpi/Core/Timestep.h"

#include <glm/glm.hpp>

/**
 *  TODO: fix drawing animating sprite from a sprite sheet, currently not correctly supported
 */

namespace Stimpi
{
	enum class AnimationState { RUNNING = 0, PAUSED, STOPPED, COMPELETED };	// Stopped - start form 0 frame; COMPELETED - used for maintaning correct UI data

	class ST_API AnimatedSprite
	{
	public:
		AnimatedSprite();
		AnimatedSprite(FilePath path);

		void SetAnimation(FilePath path);
		void SetAnimation(std::shared_ptr<Animation> animation);
		std::shared_ptr<Animation> GetAnimation() { return m_Animation; }
		
		void Update(Timestep ts);
		void SetCurrentFrame(uint32_t frame);
		void SetLooping(bool looping) { m_Looping = looping; }
		bool GetLooping() { return m_Looping; }

		void SetPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; }
		float GetPlaybackSpeed() { return m_PlaybackSpeed; }

		void Stop() { m_State = AnimationState::STOPPED; m_CurrentFrame = 0; m_TimeElapsed = 0.0f; }
		void Start() { m_State = AnimationState::RUNNING; }
		void Pause() { m_State = AnimationState::PAUSED; }

		AnimationState GetAnimationState() { return m_State; }

		bool Loaded();
		SubTexture* GetSubTexture() { return m_Animation->GetSubTexture(); }

	private:
		std::shared_ptr<Animation> m_Animation;
		AnimationState m_State = AnimationState::STOPPED;
		bool m_Looping = false;
		float m_PlaybackSpeed = 1.0f;

		// data determined from used Animation
		uint32_t m_FramesCount;

		// time and frame tracking data
		float m_TimeElapsed = 0.0f;
		float m_FrameTime = 0.0f;
		uint32_t m_CurrentFrame = 0;

	};
}