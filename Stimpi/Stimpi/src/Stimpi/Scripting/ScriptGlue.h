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
		static void RegisterDataMappings();
		static void GetFieldValue(MonoObject* object, const std::string& name, void* data);
		static void SetFieldValue(MonoObject* object, const std::string& name, void* data);
	};
}