#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <assert.h>
#include <glm/glm.hpp>

#include "Core/Core.h"

//#define NDEBUG 

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

template<>
struct fmt::formatter<glm::vec2> : fmt::formatter<std::string>
{
	auto format(glm::vec2 vec, format_context& ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "vec2[{0}, {1}]", vec.x, vec.y);
	}
};

template<>
struct fmt::formatter<glm::vec3> : fmt::formatter<std::string>
{
	auto format(glm::vec3 vec, format_context& ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "vec2[{0}, {1}, {2}]", vec.x, vec.y, vec.z);
	}
};

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

// Core - engine log macros
#define ST_CORE_TRACE(...)		Stimpi::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ST_CORE_INFO(...)		Stimpi::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ST_CORE_WARN(...)		Stimpi::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ST_CORE_ERROR(...)		Stimpi::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ST_CORE_CRITICAL(...)	Stimpi::Log::GetCoreLogger()->critical(__VA_ARGS__)

#ifndef NDEBUG
	#define ST_CORE_ASSERT_MSG(x, ...) \
	do { \
	if (x) \
		{ \
			Stimpi::Log::GetCoreLogger()->error(__VA_ARGS__); \
			std::terminate(); \
		} \
	} while (false)


	#define ST_CORE_ASSERT(x) \
	do { \
	if (x) \
		{ \
			std::terminate(); \
		} \
	} while (false)
	
#else
	#define ST_CORE_ASSERT(x, ...) if(x) { Stimpi::Log::GetCoreLogger()->error(__VA_ARGS__); }
#endif

// Client - engine log macros
#define ST_TRACE(...)		Stimpi::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ST_INFO(...)		Stimpi::Log::GetClientLogger()->info(__VA_ARGS__)
#define ST_WARN(...)		Stimpi::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ST_ERROR(...)		Stimpi::Log::GetClientLogger()->error(__VA_ARGS__)
#define ST_CRITICAL(...)	Stimpi::Log::GetClientLogger()->critical(__VA_ARGS__)

#define ST_ASSERT(x, ...) if(x) { Stimpi::Log::GetClientLogger()->error(__VA_ARGS__); }