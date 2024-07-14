#pragma once

#include "Stimpi/Scripting/ScriptEngine.h"

namespace Stimpi
{
	class ScriptFieldFragment
	{
	public:
		static void RegisterScriptFieldFunctions();

		static bool IsFieldTypeSupported(const std::string& typeName);
		static void ScriptFieldInput(ScriptObject* ownerObj, ScriptField* field);

	private:
		static std::string CallFragmentFunction(ScriptObject* ownerObj, ScriptField* field);
		static void HandlePayloadType(ScriptObject* ownerObj, ScriptField* field);
	};
}