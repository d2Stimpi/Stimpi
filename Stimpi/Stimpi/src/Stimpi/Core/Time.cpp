#include "stpch.h"
#include "Stimpi/Core/Time.h"

namespace Stimpi
{
	Time::Time()
		:m_DeltaTime(0.0f), m_TimeScale(0.0f), m_Fps(DEFAULT_FPS), m_FpsTime(0.0f)
	{

	}

	Time::~Time()
	{

	}

	Time* Time::Instance()
	{
		static auto m_Instance = std::make_unique<Time>();
		return m_Instance.get();
	}

	void Time::FrameBegin()
	{
		m_PrecedingFrameClock = GetHighPrecisionCurrentTime();
	}

	void Time::FrameEnd()
	{
		m_LastFrameClock = GetHighPrecisionCurrentTime();
		m_FloatDuration = m_LastFrameClock - m_PrecedingFrameClock;
		m_DeltaTime = m_FloatDuration.count();
	}

	void Time::FPSCapDelay()
	{
		std::chrono::duration<float, std::milli> frameTime = GetHighPrecisionCurrentTime() - m_PrecedingFrameClock;
		std::chrono::duration<float, std::milli> remFrameTime = std::chrono::duration<float, std::milli>(m_FpsTime) - frameTime;

		if (remFrameTime.count() > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(remFrameTime).count()));
		}
	}

	std::chrono::high_resolution_clock::time_point Time::GetHighPrecisionCurrentTime()
	{
		return std::chrono::high_resolution_clock::now();
	}
}