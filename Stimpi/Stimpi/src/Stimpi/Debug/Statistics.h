#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	struct ST_API FrameTimeSplit
	{
		long long m_ScriptingTime = 0ll;
		long long m_RenderingTime = 0ll;
		long long m_PhysicsSimTime = 0ll;
		long long m_IdleTime = 0ll;

		long long m_ScriptingTimeAvg = 0ll;
		long long m_RenderingTimeAvg = 0ll;
		long long m_PhysicsSimTimeAvg = 0ll;
		long long m_IdleTimeAvg = 0ll;
	};

	// TODO: Make usage under compile switchable macros

	class ST_API Statistics
	{
	public:
		static FrameTimeSplit& GetFrameTimeSplitData();

		static void SetScriptingTime(long long time);
		static void SetRenderingTime(long long time);
		static void SetPhysicsSimTime(long long time);
		static void SetIdleTime(long long time);
	};
}