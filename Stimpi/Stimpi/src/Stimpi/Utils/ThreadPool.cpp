#include "stpch.h"
#include "Stimpi/Utils/ThreadPool.h"

namespace Stimpi
{
	std::unique_ptr<ThreadPool> s_LoaderThreadPool;
	std::unique_ptr<ThreadPool> s_ScriptCompilerThreadPool;
}