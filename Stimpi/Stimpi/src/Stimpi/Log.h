#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>

#include <memory>

namespace Stimpi
{
	class ST_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core - engine log macros
#define ST_CORE_TRACE(...)		Stimpi::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ST_CORE_INFO(...)		Stimpi::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ST_CORE_WARN(...)		Stimpi::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ST_CORE_ERROR(...)		Stimpi::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ST_CORE_CRITICAL(...)	Stimpi::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client - engine log macros
#define ST_TRACE(...)		Stimpi::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ST_INFO(...)		Stimpi::Log::GetClientLogger()->info(__VA_ARGS__)
#define ST_WARN(...)		Stimpi::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ST_ERROR(...)		Stimpi::Log::GetClientLogger()->error(__VA_ARGS__)
#define ST_CRITICAL(...)	Stimpi::Log::GetClientLogger()->critical(__VA_ARGS__)