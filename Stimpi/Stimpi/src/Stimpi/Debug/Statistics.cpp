#include "stpch.h"
#include "Stimpi/Debug/Statistics.h"

namespace Stimpi
{
	struct StatisticsData
	{
		FrameTimeSplit m_FrameTimeSplit;
	};

	static StatisticsData s_Context;

	FrameTimeSplit& Statistics::GetFrameTimeSplitData()
	{
		return s_Context.m_FrameTimeSplit;
	}

	void Statistics::SetScriptingTime(long long time)
	{
		static long long totalScriptingTime = time;
		static uint32_t count = 1;
		
		if (count == 30)
		{
			s_Context.m_FrameTimeSplit.m_ScriptingTimeAvg = totalScriptingTime / count;
			count = 0;
			totalScriptingTime = 0ll;
		}
		else
		{
			totalScriptingTime += time;
		}
		count++;

		s_Context.m_FrameTimeSplit.m_ScriptingTime = time;
	}

	void Statistics::SetRenderingTime(long long time)
	{
		static long long totalRenderingTime = time;
		static uint32_t count = 1;

		if (count == 30)
		{
			s_Context.m_FrameTimeSplit.m_RenderingTimeAvg = totalRenderingTime / count;
			count = 0;
			totalRenderingTime = 0ll;
		}
		else
		{
			totalRenderingTime += time;
		}
		count++;

		s_Context.m_FrameTimeSplit.m_RenderingTime = time;
	}

	void Statistics::SetPhysicsSimTime(long long time)
	{
		static long long totalPhysicsSimTime = time;
		static uint32_t count = 1;

		if (count == 30)
		{
			s_Context.m_FrameTimeSplit.m_PhysicsSimTimeAvg = totalPhysicsSimTime / count;
			count = 0;
			totalPhysicsSimTime = 0ll;
		}
		else
		{
			totalPhysicsSimTime += time;
		}
		count++;

		s_Context.m_FrameTimeSplit.m_PhysicsSimTime = time;
	}

	void Statistics::SetIdleTime(long long time)
	{
		static long long totalIldeTime = time;
		static uint32_t count = 1;

		if (count == 30)
		{
			s_Context.m_FrameTimeSplit.m_IdleTimeAvg = totalIldeTime / count;
			count = 0;
			totalIldeTime = 0ll;
		}
		else
		{
			totalIldeTime += time;
		}
		count++;

		s_Context.m_FrameTimeSplit.m_IdleTime = time;
	}

}