#include "stpch.h"
#include "Stimpi/Core/Runtime.h"

namespace Stimpi
{
	RuntimeMode Runtime::m_RuntimeMode = RuntimeMode::APPLICATION;

	void Runtime::SetRuntimeMode(RuntimeMode mode)
	{
		m_RuntimeMode = mode;
	}

	Stimpi::RuntimeMode Runtime::GetRuntimeMode()
	{
		return m_RuntimeMode;
	}

}