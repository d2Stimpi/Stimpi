#pragma once

#include "Stimpi/Core/Core.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoMethod MonoMethod; 
	typedef struct _MonoObject MonoObject;
}

namespace Stimpi
{
	class ScriptClass;

	class ST_API ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly(const std::string& filePath);
		static void LoadClassesFromAssembly(MonoAssembly* assembly);

		static bool HasScriptClass(const std::string& className);
		static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> GetEntityClasses();
	private:
		static void InitMono();
		static void ShutdownMono();

		static MonoAssembly* LoadMonoAssembly(const std::string& assemblyPath);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		friend ScriptClass;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& namespaceName, const std::string& className);

		MonoObject* Instantiate();
		MonoMethod* GetMethod(const std::string& methodName, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params);

	private:
		std::string m_Namespace;
		std::string m_Name;

		MonoClass* m_Class = nullptr;
	};

	class ScriptInstance
	{
	public:
		ScriptInstance(std::shared_ptr<ScriptClass> scriptClass);
		~ScriptInstance() { std::cout << "destructor ScriptInstance" << std::endl; }

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);
	private:
		std::shared_ptr<ScriptClass> m_ScriptClass;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;
	};
}