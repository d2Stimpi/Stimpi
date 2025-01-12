#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Graphics/Animation/Animation.h"
#include "Stimpi/Core/Timestep.h"

#include <glm/glm.hpp>

/**
 *  TODO: fix drawing animating sprite from a sprite sheet, currently not correctly supported
 * 
 *  TODO: add WrapModes: ClampForever, Once, Loop
 */

namespace Stimpi
{
	enum class AnimationState { RUNNING = 0, PAUSED, STOPPED, COMPELETED };	// Stopped - start form 0 frame; COMPELETED - used for maintaning correct UI data
	enum class AnimationWrapMode { ONCE = 0, LOOPING, CLAMP };	// Clamp - show last frame when animation is complete

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
		void SetLooping(bool looping) { looping ? m_WrapMode = AnimationWrapMode::LOOPING : m_WrapMode = AnimationWrapMode::ONCE; }
		bool GetLooping() { return m_WrapMode == AnimationWrapMode::LOOPING; }

		void SetPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; }
		float GetPlaybackSpeed() { return m_PlaybackSpeed; }

		void Stop() { m_State = AnimationState::STOPPED; m_CurrentFrame = 0; m_TimeElapsed = 0.0f; }
		void Start() { m_State = AnimationState::RUNNING; }
		void Pause() { m_State = AnimationState::PAUSED; }

		AnimationState GetAnimationState() { return m_State; }
		AnimationWrapMode GetWrapMode() { return m_WrapMode; }
		void SetWrapMode(AnimationWrapMode wrapMode) { m_WrapMode = wrapMode; }

		SubTexture* GetSubTexture() { return m_Animation->GetSubTexture(); }

	private:
		std::shared_ptr<Animation> m_Animation;
		AnimationState m_State = AnimationState::STOPPED;
		AnimationWrapMode m_WrapMode = AnimationWrapMode::ONCE;
		float m_PlaybackSpeed = 1.0f;

		// data determined from used Animation
		uint32_t m_FramesCount = 0;

		// time and frame tracking data
		float m_TimeElapsed = 0.0f;
		float m_FrameDuration = 0.0f;
		uint32_t m_CurrentFrame = 0;

	};
}