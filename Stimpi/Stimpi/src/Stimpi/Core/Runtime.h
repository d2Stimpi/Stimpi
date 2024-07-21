#pragma once

#include "Stimpi/Core/Core.h"

#define SET_EDITOR_RUNTIME_MODE	Stimpi::Runtime::SetRuntimeMode(RuntimeMode::EDITOR);

namespace Stimpi
{
	enum class RuntimeMode { APPLICATION = 0, EDITOR };

	class ST_API Runtime
	{
	public:
		static void SetRuntimeMode(RuntimeMode mode);
		static RuntimeMode GetRuntimeMode();

	private:
		static RuntimeMode m_RuntimeMode;
	};

}