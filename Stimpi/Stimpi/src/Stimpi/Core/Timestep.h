#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	/* Time by default in milliseconds, impl cast will be in seconds */
	class ST_API Timestep
	{
	public:
		Timestep(float time = 0.0f)
			: m_Timestep(time)
		{
		}

		float GetSeconds() { return m_Timestep * 0.001f; }
		float GetMilliseconds() { return m_Timestep ; }

		// Used to implement stepping frame per frame
		void SetTime(float timestep) { m_Timestep = timestep; };

		operator float() const { return m_Timestep * 0.001f; }

	private:
		float m_Timestep;
	};
}