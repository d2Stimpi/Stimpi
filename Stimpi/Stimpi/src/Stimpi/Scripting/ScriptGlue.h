#pragma once

extern "C"
{
	typedef struct _MonoObject MonoObject;
}

namespace Stimpi
{
	class ScriptGlue
	{
	public:
		static void RegisterFucntions();
		static void RegisterComponents();
	};
}