#include "stpch.h"
#include "Stimpi/Core/Time.h"

namespace Stimpi
{
	Time::Time()
		:m_DeltaTime(0.0f), m_TimeScale(0.0f), m_Fps(DEFAULT_FPS), m_FpsTime(1000.0f/DEFAULT_FPS)
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


	std::chrono::time_point<std::chrono::steady_clock> Clock::m_StartTimepoint;

	void Clock::Begin()
	{
		m_StartTimepoint = std::chrono::steady_clock::now();
	}

	long long Clock::Stop()
	{
		auto endTimepoint = std::chrono::steady_clock::now();
		auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

		return elapsedTime.count();
	}

}