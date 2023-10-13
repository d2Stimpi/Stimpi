#pragma once

#include "Stimpi/Core/Core.h"

#define DEFAULT_FPS	60

namespace Stimpi
{
	class ST_API Time
	{
	public:
		Time();
		~Time();

		static Time* Instance();

		//Return time that past since last frame and the one preceeding it
		float DeltaTime() { return m_DeltaTime; }
		float TimeScale() { return m_TimeScale; }
		void SetTimeScale(float scale) { m_TimeScale = scale; }
		void SetFPS(int fps) { m_Fps = fps; m_FpsTime = 1000.0f / m_Fps; }
		float GetFPS() { return m_Fps; }

		//Return actual average fps over 10 frames
		float GetActiveFPS() { return 1000.0f / m_DeltaTime; }

		//Used to indicate when the Frame begins/ends to calculate deltaTime. Should not be used outside of Main Game Loop
		void FrameBegin();
		void FrameEnd();
		void FPSCapDelay();

	private:
		std::chrono::high_resolution_clock::time_point GetHighPrecisionCurrentTime();

		float m_DeltaTime;
		float m_TimeScale; // 0.0f - 1.0f is a valid value
		uint32_t m_Fps;
		float m_FpsTime;
		//High precision
		std::chrono::high_resolution_clock::time_point m_LastFrameClock;
		std::chrono::high_resolution_clock::time_point m_PrecedingFrameClock;
		std::chrono::duration<float, std::milli> m_FloatDuration;
	};
}