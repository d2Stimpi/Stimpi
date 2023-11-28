#include "stpch.h"
#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/SystemUtils.h"
#include "Stimpi/Scripting/ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"

namespace Stimpi
{
	class Utils
	{
	public:

		static void PrintAssemblyTypes(MonoAssembly* assembly)
		{
			MonoImage* image = mono_assembly_get_image(assembly);
			const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
			int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

			ST_CORE_INFO("=== PrintAssemblyTypes ===");
			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				ST_CORE_INFO("{0}.{1}", nameSpace, name);
			}
			ST_CORE_INFO("==========================");
		}

		static bool CheckMonoError(MonoError& error)
		{
			bool hasError = !mono_error_ok(&error);
			if (hasError)
			{
				unsigned short errorCode = mono_error_get_error_code(&error);
				const char* errorMessage = mono_error_get_message(&error);
				ST_CORE_ERROR("Mono Error: {} - {}", errorCode, errorMessage);
				mono_error_cleanup(&error);
			}
			return hasError;
		}

		static std::string MonoStringToUTF8(MonoString* monoString)
		{
			if (monoString == nullptr || mono_string_length(monoString) == 0)
				return "";

			MonoError error;
			char* utf8 = mono_string_to_utf8_checked(monoString, &error);
			if (CheckMonoError(error))
				return "";

			std::string result(utf8);
			mono_free(utf8);

			return result;
		}
	};

	struct ScriptEngineData
	{
		MonoDomain* m_RootDomain = nullptr;
		MonoDomain* m_AppDomain = nullptr;

		MonoAssembly* m_CoreAssembly = nullptr;
		MonoImage* m_CoreAssemblyImage = nullptr;

		ScriptClass m_EntityClass; // Used for Entity ctor
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> m_EntityClasses;
	};

	static ScriptEngineData* s_Data;
	

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData;

		InitMono();
		LoadAssembly("../resources/scripts/Stimpi-ScriptCore.dll");

		Utils::PrintAssemblyTypes(s_Data->m_CoreAssembly);
		LoadClassesFromAssembly(s_Data->m_CoreAssembly);

		s_Data->m_EntityClass = ScriptClass("Stimpi", "Entity");

		ScriptGlue::RegisterFucntions();
		ScriptGlue::RegosterComponents();

#if 0
		// Load the CS class
		s_Data->m_EntityClass = ScriptClass("Stimpi", "Entity");

		// Allocate instance of CS class
		MonoObject* classObjInstance = s_Data->m_EntityClass.Instantiate();

		// Get a method
		MonoMethod* printFloatMethod = s_Data->m_EntityClass.GetMethod("PrintFloatVar", 0);
		s_Data->m_EntityClass.InvokeMethod(classObjInstance, printFloatMethod, nullptr);

		// Example - call method with a param
		MonoMethod* doStuffMethod = s_Data->m_EntityClass.GetMethod("DoSomeStuff", 1);
		float value = 10.0f;
		void* param = &value;
		s_Data->m_EntityClass.InvokeMethod(classObjInstance, doStuffMethod, &param);
		s_Data->m_EntityClass.InvokeMethod(classObjInstance, printFloatMethod, nullptr);

		// Read field 
		/*MonoClassField* floatField = mono_class_get_field_from_name(csClass, "PublicFloatVar");
		value = 8.0f;
		mono_field_set_value(classObjInstance, floatField, &value);
		mono_runtime_invoke(printFloatMethod, classObjInstance, nullptr, &exception);
		*/

		MonoMethod* printMsgMethod = s_Data->m_EntityClass.GetMethod("PrintMessage", 2);
		MonoString* msg = mono_string_new(s_Data->m_AppDomain, "Hello there C#");
		float param2 = 11.0f;
		void* params[] = { msg, &param2 };
		s_Data->m_EntityClass.InvokeMethod(classObjInstance, printMsgMethod, params);

		// Internal call example
		//mono_add_internal_call("Stimpi.Hello::Sample", Sample);
		MonoMethod* callNativeMethod = s_Data->m_EntityClass.GetMethod("CallNativeCode", 0);
		s_Data->m_EntityClass.InvokeMethod(classObjInstance, callNativeMethod, nullptr);
#endif
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();
		delete s_Data;
		s_Data = nullptr;
	}

	void ScriptEngine::LoadAssembly(const std::string& filePath)
	{
		// Create an App Domain
		s_Data->m_AppDomain = mono_domain_create_appdomain("StimpiScriptRuntime", nullptr);
		mono_domain_set(s_Data->m_AppDomain, true);

		s_Data->m_CoreAssembly = LoadMonoAssembly(filePath);
		s_Data->m_CoreAssemblyImage = mono_assembly_get_image(s_Data->m_CoreAssembly);
	}

	void ScriptEngine::LoadClassesFromAssembly(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = GetClassInAssembly(assembly, "Stimpi", "Entity");

		for (int32_t i = 0; i < numTypes; i++)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			std::string fullName;
			if (strlen(nameSpace) == 0)
				fullName = name;
			else
				fullName = fmt::format("{}.{}", nameSpace, name);

			MonoClass* monoClass = GetClassInAssembly(assembly, nameSpace, name);
			if (entityClass == monoClass)
				continue;

			bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
			if (isEntity)
				s_Data->m_EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);
		}
	}

	bool ScriptEngine::HasScriptClass(const std::string& className)
	{
		return s_Data->m_EntityClasses.find(className) != s_Data->m_EntityClasses.end();
	}

	std::unordered_map<std::string, std::shared_ptr<Stimpi::ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->m_EntityClasses;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib/4.5");

		MonoDomain* rootDomain = mono_jit_init("StimpiJitRuntime");
		ST_CORE_ASSERT(rootDomain == nullptr, "Invalid Mono JIT Domain");

		// Store the root domain pointer
		s_Data->m_RootDomain = rootDomain;
	}

	void ScriptEngine::ShutdownMono()
	{
		s_Data->m_AppDomain = nullptr;
		s_Data->m_RootDomain = nullptr;
	}

	MonoAssembly* ScriptEngine::LoadMonoAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = FileSystem::ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			ST_CORE_ERROR("LoadCSharpAssembly error: {}", errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		// Don't forget to free the file data
		delete[] fileData;

		return assembly;
	}	

	MonoClass* ScriptEngine::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, namespaceName, className);

		if (klass == nullptr)
		{
			ST_CORE_ERROR("GetClassInAssembly error loading class {}.{}", namespaceName, className);
			return nullptr;
		}

		return klass;
	}

	MonoObject* ScriptEngine::InstantiateClass(MonoClass* monoClass)
	{
		MonoObject* instance = mono_object_new(s_Data->m_AppDomain, monoClass);
		if (instance == nullptr)
			ST_CORE_ERROR("[ScriptClass] Error instantiating class instance");

		// Call parameterless constructor
		mono_runtime_object_init(instance);

		return instance;
	}

	MonoImage* ScriptEngine::GetCoreAssemblyImage()
	{
		return s_Data->m_CoreAssemblyImage;
	}

	/* ======== ScriptClass ======== */

	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
		: m_Namespace(namespaceName), m_Name(className)
	{
		m_Class = ScriptEngine::GetClassInAssembly(s_Data->m_CoreAssembly, namespaceName.c_str(), className.c_str());
	}

	MonoObject* ScriptClass::Instantiate()
	{
		return ScriptEngine::InstantiateClass(m_Class);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int parameterCount)
	{
		MonoMethod* monoMethod = mono_class_get_method_from_name(m_Class, methodName.c_str(), parameterCount);
		if (monoMethod == nullptr)
		{
			ST_CORE_ERROR("[ScriptClass] Error finding method {}", methodName);
		}

		return monoMethod;
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception;
		mono_runtime_invoke(method, instance, params, &exception);

		return nullptr;
	}

	/* ======== ScriptInstance ======== */

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass), m_Entity(entity)
	{
		m_Instance = m_ScriptClass->Instantiate();
		m_Constructor = s_Data->m_EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

		void* param = &entity;
		m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
	}

	ScriptInstance::~ScriptInstance()
	{
		ST_CORE_TRACE("Destroy ScriptInstance");
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod, nullptr);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		void* param = &ts;
		if (m_OnUpdateMethod)
			m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod, &param);
	}

}