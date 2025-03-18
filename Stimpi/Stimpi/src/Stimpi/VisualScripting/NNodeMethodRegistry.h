#pragma once

#include "Stimpi/Core/Core.h"

namespace Stimpi
{
	struct Method;

	enum class MethodName
	{
		None,
#define DEFINE_NODE_NAME(name)	name,
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)
	};

	using ObjectParam = void*;
	using Param = std::variant<uint32_t, bool, int, float, glm::vec2, glm::vec3, glm::vec4, ObjectParam>;
	using MethodType = std::function<bool(Method*)>;
	using MethodRegistry = std::unordered_map<MethodName, MethodType>;

	class ST_API NNodeMethodRegistry
	{
	public:
		static void PopulateMethods();

		static void RegisterMethod(MethodName name, MethodType method);
		static MethodType GetMethod(MethodName name);

		static std::string MethodNameToString(MethodName name);
		static MethodName StringToMethodName(const std::string& str);
	};
}