#include "stpch.h"
#include "Stimpi/Scripting/ScriptGlue.h"

#include "Stimpi/Log.h"
#include "mono/metadata/appdomain.h"

#define ST_ADD_INTERNAL_CALL(Name)	mono_add_internal_call("Stimpi.InternalCalls::" #Name, Name);

namespace Stimpi
{
	static void Sample()
	{
		ST_CORE_INFO("C# -> Hello from C++");
	}

	void ScriptGlue::RegisterFucntions()
	{
		// mono_add_internal_call
		ST_ADD_INTERNAL_CALL(Sample);
	}

}