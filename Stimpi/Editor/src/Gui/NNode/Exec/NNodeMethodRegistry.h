#pragma once

#include "Gui/NNode/Exec/ExecTree.h"

namespace Stimpi
{
	using MethodRegistry = std::unordered_map<MethodName, MethodType>;

	class NNodeMethodRegistry
	{
	public:
		static void PopulateMethods();

		static void RegisterMethod(MethodName name, MethodType method);
		static MethodType GetMethod(MethodName name);

	private:
		static MethodRegistry m_MethodRegistry;
	};
}