#include "stpch.h"
#include "Stimpi/Utils/ThreadPool.h"

namespace Stimpi
{
	// Asset importer threads
	std::unique_ptr<ThreadPool> s_AssetImporterPool;

	std::unique_ptr<ThreadPool> s_LoaderThreadPool;
	std::unique_ptr<ThreadPool> s_ScriptCompilerThreadPool;
}