#include "stpch.h"
#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Log.h"
#include "Stimpi/Utils/SystemUtils.h"

#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Component.h"

#include "Stimpi/Scripting/ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/attrdefs.h"

namespace Stimpi
{
	enum class Accessibility : uint8_t
	{
		NONE = 0,
		PRIVATE = (1 << 0),
		INTERNAL = (1 << 1),
		PROTECTED = (1 << 2),
		PUBLIC = (1 << 3)
	};

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

		static uint8_t GetFieldAccessibility(MonoClassField* field)
		{
			uint8_t accessibility = (uint8_t)Accessibility::NONE;
			uint32_t accessFlag = mono_field_get_flags(field) & MONO_FIELD_ATTR_FIELD_ACCESS_MASK;

			switch (accessFlag)
			{
				case MONO_FIELD_ATTR_PRIVATE:
				{
					accessibility = (uint8_t)Accessibility::PRIVATE;
					break;
				}
				case MONO_FIELD_ATTR_FAM_AND_ASSEM:
				{
					accessibility |= (uint8_t)Accessibility::PROTECTED;
					accessibility |= (uint8_t)Accessibility::INTERNAL;
					break;
				}
				case MONO_FIELD_ATTR_ASSEMBLY:
				{
					accessibility = (uint8_t)Accessibility::INTERNAL;
					break;
				}
				case MONO_FIELD_ATTR_FAMILY:
				{
					accessibility = (uint8_t)Accessibility::PROTECTED;
					break;
				}
				case MONO_FIELD_ATTR_FAM_OR_ASSEM:
				{
					accessibility |= (uint8_t)Accessibility::PRIVATE;
					accessibility |= (uint8_t)Accessibility::PROTECTED;
					break;
				}
				case MONO_FIELD_ATTR_PUBLIC:
				{
					accessibility = (uint8_t)Accessibility::PUBLIC;
					break;
				}
			}

			return accessibility;
		}

		static uint8_t GetPropertyAccessibility(MonoProperty* property)
		{
			uint8_t accessibility = (uint8_t)Accessibility::NONE;

			// Get a reference to the property's getter method
			MonoMethod* propertyGetter = mono_property_get_get_method(property);
			if (propertyGetter != nullptr)
			{
				// Extract the access flags from the getters flags
				uint32_t accessFlag = mono_method_get_flags(propertyGetter, nullptr) & MONO_METHOD_ATTR_ACCESS_MASK;

				switch (accessFlag)
				{
					case MONO_FIELD_ATTR_PRIVATE:
					{
						accessibility = (uint8_t)Accessibility::PRIVATE;
						break;
					}
					case MONO_FIELD_ATTR_FAM_AND_ASSEM:
					{
						accessibility |= (uint8_t)Accessibility::PROTECTED;
						accessibility |= (uint8_t)Accessibility::INTERNAL;
						break;
					}
					case MONO_FIELD_ATTR_ASSEMBLY:
					{
						accessibility = (uint8_t)Accessibility::INTERNAL;
						break;
					}
					case MONO_FIELD_ATTR_FAMILY:
					{
						accessibility = (uint8_t)Accessibility::PROTECTED;
						break;
					}
					case MONO_FIELD_ATTR_FAM_OR_ASSEM:
					{
						accessibility |= (uint8_t)Accessibility::PRIVATE;
						accessibility |= (uint8_t)Accessibility::PROTECTED;
						break;
					}
					case MONO_FIELD_ATTR_PUBLIC:
					{
						accessibility = (uint8_t)Accessibility::PUBLIC;
						break;
					}
				}
			}
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

		/* Scene data - Class Instances per entity */
		std::shared_ptr<Scene> m_Scene;
		std::unordered_map<uint32_t, std::shared_ptr<ScriptInstance>> m_EntityInstances;
	};

	static ScriptEngineData* s_Data;
	

	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData;
		s_Data->m_Scene = SceneManager::Instance()->GetActiveSceneRef();

		OnSceneChangedListener onScneeChanged = [&]() {
			s_Data->m_Scene = SceneManager::Instance()->GetActiveSceneRef();
			// Re-create Script instances
			CreateScriptInstances();
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);

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

	std::shared_ptr<ScriptClass> ScriptEngine::GetScriptClassByName(const std::string& className)
	{
		if (s_Data->m_EntityClasses.find(className) != s_Data->m_EntityClasses.end())
		{
			return s_Data->m_EntityClasses.find(className)->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::unordered_map<std::string, std::shared_ptr<Stimpi::ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->m_EntityClasses;
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path("mono/lib/4.5");

		MonoDomain* rootDomain = mono_jit_init("StimpiJitRuntime");
		ST_CORE_ASSERT_MSG(rootDomain == nullptr, "Invalid Mono JIT Domain");

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

	MonoDomain* ScriptEngine::GetAppDomain()
	{
		return s_Data->m_AppDomain;
	}

	/*
	*  Called each time Scene changes. Instantiate ScriptClasses that are used by Entities in the scene.
	*  
	*/
	void ScriptEngine::CreateScriptInstances()
	{
		s_Data->m_EntityInstances.clear();

		auto entities = s_Data->m_Scene->m_Entities;
		for (Entity entity : entities)
		{
			if (entity.HasComponent<ScriptComponent>())
			{
				auto scriptComponent = entity.GetComponent<ScriptComponent>();
				auto scriptClass = GetScriptClassByName(scriptComponent.m_ScriptName);
				s_Data->m_EntityInstances[entity] = std::make_shared<ScriptInstance>(scriptClass, entity);
			}
		}
	}

	void ScriptEngine::OnScenePlay()
	{
		for (auto element : s_Data->m_EntityInstances)
		{
			auto instance = element.second;
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnSceneUpdate(Timestep ts)
	{
		for (auto element : s_Data->m_EntityInstances)
		{
			auto instance = element.second;
			instance->InvokeOnUpdate(ts);
		}
	}

	void ScriptEngine::OnSceneStop()
	{
		s_Data->m_EntityInstances.clear();
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::CreateScriptInstance(const std::string& className, Entity entity)
	{
		auto scriptClass = GetScriptClassByName(className);
		if (scriptClass == nullptr)
			return nullptr;

		return std::make_shared<ScriptInstance>(scriptClass, entity);
	}

	std::shared_ptr<Stimpi::ScriptInstance> ScriptEngine::GetScriptInstance(Entity entity)
	{
		if (s_Data->m_EntityInstances.find(entity) != s_Data->m_EntityInstances.end())
		{
			return s_Data->m_EntityInstances.at(entity);
		}
		else
		{
			return nullptr;
		}
	}

	std::string ScriptEngine::GetFieldName(MonoClassField* field)
	{
		return mono_field_get_name(field);
	}

	/* ======== ScriptClass ======== */

	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className)
		: m_Namespace(namespaceName), m_Name(className)
	{
		m_Class = ScriptEngine::GetClassInAssembly(s_Data->m_CoreAssembly, namespaceName.c_str(), className.c_str());
		ST_CORE_ASSERT(!m_Class);

		LoadFields();
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

	MonoClassField* ScriptClass::GetField(const std::string& fieldName)
	{
		return mono_class_get_field_from_name(m_Class, fieldName.c_str());
	}

	void ScriptClass::LoadFields()
	{
		m_Fields.clear();
		ST_CORE_INFO("Class: {}", mono_class_get_name(m_Class));

		void* iter = nullptr;
		MonoClassField* field;
		while (field = mono_class_get_fields(m_Class, &iter))
		{
			MonoType* type = mono_field_get_type(field);
			// Check if accessibility is public
			uint8_t vis = Utils::GetFieldAccessibility(field);
			if (vis & (uint8_t)Accessibility::PUBLIC)
			{
				m_Fields.push_back(field);
				auto ttype = mono_type_get_type(type);
				ST_CORE_INFO("Field:  {}", mono_field_get_name(field));
				ST_CORE_INFO("Field: type {}, accessability {}", ttype, Utils::GetFieldAccessibility(field));
			}
		}
	}

	void ScriptClass::LoadProperties()
	{

	}

	/* ======== ScriptInstance ======== */

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity)
		: m_ScriptClass(scriptClass), m_Entity(entity)
	{
		m_Instance = m_ScriptClass->Instantiate();
		m_Constructor = s_Data->m_EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

		/* Populate Fields found in ScriptClass */
		auto fields = m_ScriptClass->GetAllFields();
		for (auto field : fields)
		{
			auto scriptField = std::make_shared<ScriptField>(this, field);
			m_ScriptFields.push_back(scriptField);
		}

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

	std::shared_ptr<Stimpi::ScriptField> ScriptInstance::GetScriptFieldFromMonoField(MonoClassField* field)
	{
		for (auto item : m_ScriptFields)
		{
			if (item->GetMonoField() == field)
				return item;
		}
		return nullptr;
	}

	/* ======== ScriptField ======== */

	ScriptField::ScriptField(ScriptInstance* instance, MonoClassField* field)
		: m_Instance(instance), m_MonoField(field)
	{
		MonoType* monoType = mono_field_get_type(field);
		uint32_t dataType = mono_type_get_type(monoType);
		switch (dataType)
		{
		case MONO_TYPE_R4:

			break;
		default:
			break;
		}
	}

	// TODO: reading only initial value from C# and store it locally in ScriptEngine - rework stuff
	void ScriptField::ReadFieldValue(void* value)
	{
		mono_field_get_value(m_Instance->GetInstance(), m_MonoField, value);
	}

	void ScriptField::SetFieldValue(void* value)
	{
		mono_field_set_value(m_Instance->GetInstance(), m_MonoField, value);
	}

}