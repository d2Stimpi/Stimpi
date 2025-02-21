#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/VisualScripting/ExecTree.h"

namespace Stimpi
{
	using MethodRegistry = std::unordered_map<MethodName, MethodType>;

	class ST_API NNodeMethodRegistry
	{
	public:
		static void PopulateMethods();

		static void RegisterMethod(MethodName name, MethodType method);
		static MethodType GetMethod(MethodName name);
	};
}