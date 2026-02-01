#include "stpch.h"
#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Core/Time.h"
#include "Stimpi/Debug/Statistics.h"
#include "Stimpi/Utils/SystemUtils.h"
#include "Stimpi/Utils/FileWatcher.h"

#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Component.h"

#include "Stimpi/Scripting/ScriptGlue.h"
#include "Stimpi/Scripting/ScriptGlueTypes.h"

#include "mono/jit/jit.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/attrdefs.h"
#include "mono/metadata/mono-gc.h"
#include "mono/metadata/mono-config.h"

/** TODO: fix issues:
 *  - Component adds a Script -> Script needs to be instantiated and so. Atm it won't be "active" before scene is reloaded
 *  - Editor crash when adding new script and attemtping to read Fields; Related to above issue.
 * 
 *  Features
 *  - Find a better way to handle Field & Property data, keeping a local value, type handling...
 */

#define SCRIPTENGINE_DBG	(false)

#define MONO_LIB_PATH	"../mono/lib/mono/4.5"

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

			if (SCRIPTENGINE_DBG) ST_CORE_INFO("=== PrintAssemblyTypes ===");
			for (int32_t i = 0; i < numTypes; i++)
			{
				uint32_t cols[MONO_TYPEDEF_SIZE];
				mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

				const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
				const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

				if (SCRIPTENGINE_DBG) ST_CORE_INFO("{0}.{1}", nameSpace, name);
			}
			if (SCRIPTENGINE_DBG) ST_CORE_INFO("==========================");
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

		/**
		 * Mono types  -> Cpp types
		 *
		 * MONO_TYPE_I
		 * MONO_TYPE_U		-> pointer ?
		 *
		 * MONO_TYPE_BOOLEAN
		 * MONO_TYPE_CHAR
		 * MONO_TYPE_I1		-> int8_t
		 * MONO_TYPE_U1		-> uint8_t
		 *
		 * MONO_TYPE_I2		-> int16_t
		 * MONO_TYPE_U2		-> uint16_t
		 *
		 * MONO_TYPE_I4		-> int32_t
		 * MONO_TYPE_U4		-> uint32_t
		 *
		 * MONO_TYPE_STRING	-> mono string
		 *
		 * MONO_TYPE_CLASS
		 * MONO_TYPE_OBJECT
		 *
		 * MONO_TYPE_PTR
		 * MONO_TYPE_FNPTR
		 * MONO_TYPE_ARRAY
		 * MONO_TYPE_SZARRAY
		 *
		 * MONO_TYPE_I8		-> int64_t
		 * MONO_TYPE_U8		-> uint64_t
		 * MONO_TYPE_R4		-> float
		 * MONO_TYPE_R8		-> double
		 */

		static FieldType GetFieldTypeFromMonoType(uint32_t monoType)
		{
			switch (monoType)
			{
			case MONO_TYPE_R4:			return FieldType::FIELD_TYPE_FLOAT;
			case MONO_TYPE_U4:			return FieldType::FIELD_TYPE_UINT;
			case MONO_TYPE_I4:			return FieldType::FIELD_TYPE_INT;
			case MONO_TYPE_CLASS:		return FieldType::FIELD_TYPE_CLASS;
			case MONO_TYPE_VALUETYPE:	return FieldType::FIELD_TYPE_STRUCT;
			}

			return FieldType::UNKNOWN;
		}
	};

	/**
	 * Data represents one "VisualScript" asset
	 * - only one Entity class is expected
	 */
	struct AssetAssembly
	{
		MonoAssembly* m_Assembly = nullptr;
		MonoImage* m_AssemblyImage = nullptr;

		std::string m_ScriptClassName;
		std::shared_ptr<ScriptClass> m_EntityClass;
	};

	struct ScriptEngineData
	{
		MonoDomain* m_RootDomain = nullptr;
		MonoDomain* m_AppDomain = nullptr;

		MonoAssembly* m_CoreAssembly = nullptr;
		MonoImage* m_CoreAssemblyImage = nullptr;

		MonoAssembly* m_ClientAssembly = nullptr;
		MonoImage* m_ClientAssemblyImage = nullptr;

		ScriptClass m_EntityClass; // Used for the Entity class;
		std::vector<std::string> m_ScriptClassNames;
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> m_EntityClasses;

		// Scene data - Class Instances per entity
		std::shared_ptr<Scene> m_Scene;
		std::unordered_map<uint32_t, std::shared_ptr<ScriptInstance>> m_EntityInstances;
		std::vector<uint32_t> m_EntitiesToRemove;

		// Ordered set of Entity Instances
		// TODO: properly organize Instances by ScriptOrder value
		// - temp workaround: Only types that define attribute ScriptOrder will end up in priority set
		std::unordered_map<uint32_t, std::shared_ptr<ScriptInstance>> m_UnorderedEntityInstances;
		std::vector<std::shared_ptr<ScriptInstance>> m_PriorityEntityInstances;

		// Custom Non-Entity classes
		std::vector<std::string> m_ClassNames;
		std::unordered_map<std::string, std::shared_ptr<ScriptClass>> m_Classes;

		// "VisualScripting" asset scripts
		std::unordered_map<std::string, std::shared_ptr<AssetAssembly>> m_AssetAssemblies;

		// Script paths
		std::filesystem::path m_CoreScirptPath;
		std::filesystem::path m_ClientScirptPath;
		std::filesystem::path m_AssetsScriptPath;

		FileWatchListener m_OnScriptUpdated;
		FileWatchListener m_OnAssetScriptsUpdated;	// Track script assets (generated from VisualScripting)
		bool m_DeferreAsemblyReload = false;	// Reload assembly if script change was detected and scene was running

		// CustomAttributes.cs class instance
		std::shared_ptr<ScriptInstance> m_AttributeLookup;
	};

	static ScriptEngineData* s_Data;
	static std::string s_CoreScriptName = "Stimpi-ScriptCore.dll";

	static std::string s_ClientScriptName = "Sandbox-Script.dll";

	static uint32_t s_GCHandleCount = 0;

	static uint32_t GetGCHandle(MonoObject* obj, bool pinned)
	{
		s_GCHandleCount++;
		return mono_gchandle_new(obj, pinned);
	}

	static void FreeGCHandle(uint32_t handle)
	{
		s_GCHandleCount--;
		mono_gchandle_free(handle);
	}


	void ScriptEngine::Init()
	{
		s_Data = new ScriptEngineData;
		s_Data->m_Scene = SceneManager::Instance()->GetActiveSceneRef();

		OnSceneChangedListener onScneeChanged = [&]() {
			s_Data->m_Scene = SceneManager::Instance()->GetActiveSceneRef();
		};
		SceneManager::Instance()->RegisterOnSceneChangeListener(onScneeChanged);

		// Note: full absolute path required for file watcher
		s_Data->m_CoreScirptPath = std::filesystem::absolute(ResourceManager::GetScriptsPath()) / s_CoreScriptName;
		s_Data->m_ClientScirptPath = std::filesystem::absolute(ResourceManager::GetScriptsPath()) / s_ClientScriptName;
		s_Data->m_AssetsScriptPath = std::filesystem::absolute(ResourceManager::GetAssetsPath()) / "scripts";

		// Reserve space
		s_Data->m_PriorityEntityInstances.reserve(64);

		InitMono();
		LoadAssembly();

		LoadAssetAsseblies();

		Utils::PrintAssemblyTypes(s_Data->m_CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->m_ClientAssembly);
		LoadClassesFromAssembly(s_Data->m_CoreAssembly);
		LoadClassesFromAssembly(s_Data->m_ClientAssembly);

		/**
		 * Load classes that are used internally for retrieving data from C#,
		 * such as field Attributes.
		 */
		LoadInternalClasses();

		s_Data->m_EntityClass = ScriptClass("Stimpi", "Entity", s_Data->m_CoreAssembly);

		ScriptGlue::RegisterFucntions();
		ScriptGlue::RegisterComponents();
		ScriptSeriaizer::RegisterSirializableTypes();

		// Register Hot reload watcher
		s_Data->m_OnScriptUpdated = [](SystemShellEvent* event)
		{
			ST_CORE_INFO("ScriptEngine - sh event {}", (int)event->GetType());
			if (s_Data->m_Scene->GetRuntimeState() == RuntimeState::STOPPED)
			{
				// TODO: check double event trigger on copy/paste?
				// Missing script fields init after reload of dlls (when scene is loaded, fields are populated)
				if (event->GetType() == SystemShellEventType::SH_UPDATED)
				{
					ReloadAssembly();
				}
			}
			else
			{
				s_Data->m_DeferreAsemblyReload = true;
			}
		};

		s_Data->m_OnAssetScriptsUpdated = [](SystemShellEvent * event)
		{
			if (event->GetType() == SystemShellEventType::SH_UPDATED)
			{
				ST_CORE_INFO("ScriptEngine: SH_UPDATED - {}", event->GetFilePath());
				ReloadAssetAssembly(event->GetFilePath());
			}

			if (event->GetType() == SystemShellEventType::SH_CREATED)
			{
				ST_CORE_INFO("ScriptEngine: SH_CREATED - {}", event->GetFilePath());
				ReloadAssetAssembly(event->GetFilePath());
			}

			if (event->GetType() == SystemShellEventType::SH_RENAMED)
			{
				ST_CORE_INFO("ScriptEngine: event SH_RENAMED - {} -> {}", event->GetFilePath(), event->GetNewFilePath());
			}

			if (event->GetType() == SystemShellEventType::SH_DELETED)
			{
				ST_CORE_INFO("ScriptEngine: event SH_DELETED - {}", event->GetFilePath());
			}
		};

		FileWatcher::AddWatcher(s_Data->m_CoreScirptPath, s_Data->m_OnScriptUpdated);
		FileWatcher::AddWatcher(s_Data->m_ClientScirptPath, s_Data->m_OnScriptUpdated);
		FileWatcher::AddWatcher(s_Data->m_AssetsScriptPath, s_Data->m_OnAssetScriptsUpdated);
	}

	void ScriptEngine::Shutdown()
	{
		ShutdownMono();

		FileWatcher::RemoveWatcher(s_Data->m_CoreScirptPath);
		FileWatcher::RemoveWatcher(s_Data->m_ClientScirptPath);
		FileWatcher::RemoveWatcher(s_Data->m_AssetsScriptPath);

		s_Data = nullptr;
		delete s_Data;
	}

	void ScriptEngine::LoadAssembly()
	{
		// Create an App Domain
		s_Data->m_AppDomain = mono_domain_create_appdomain("StimpiScriptRuntime", nullptr);
		mono_domain_set(s_Data->m_AppDomain, true);

		s_Data->m_CoreAssembly = LoadMonoAssembly(GetCoreScriptPath().string());
		s_Data->m_CoreAssemblyImage = mono_assembly_get_image(s_Data->m_CoreAssembly);
		// Load Client Assembly
		s_Data->m_ClientAssembly = LoadMonoAssembly(GetClientScriptPath().string());
		s_Data->m_ClientAssemblyImage = mono_assembly_get_image(s_Data->m_ClientAssembly);
	}


	void ScriptEngine::LoadAssetAsseblies()
	{
		for (auto& directoryEntry : std::filesystem::directory_iterator(s_Data->m_AssetsScriptPath))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, ResourceManager::GetAssetsPath());
			std::string filenameStr = relativePath.filename().string();

			ST_CORE_INFO("Loading Asset script - {}", filenameStr);

			auto assetAssembly = std::make_shared<AssetAssembly>();
			assetAssembly->m_Assembly = LoadMonoAssembly(path.string());
			assetAssembly->m_AssemblyImage = mono_assembly_get_image(assetAssembly->m_Assembly);

			LoadClassesFromAssetAssembly(assetAssembly.get());
			s_Data->m_AssetAssemblies[filenameStr] = assetAssembly;
		}
	}

	void ScriptEngine::UnloadAssembly()
	{
		if (s_Data->m_AppDomain != nullptr)
		{
			mono_domain_set(s_Data->m_RootDomain, true);
			//mono_domain_unload(s_Data->m_AppDomain);

			mono_gc_collect(mono_gc_max_generation());
			mono_domain_finalize(s_Data->m_AppDomain, 2000);
			mono_gc_collect(mono_gc_max_generation());

			MonoException* exc = nullptr;
			mono_domain_try_unload(s_Data->m_AppDomain, (MonoObject**)&exc);

			if (exc) {
				ST_CORE_ERROR("Exception thrown when unloading domain");
			}

			// Clear internal data. TODO: handle multiple DLLs
			s_Data->m_ScriptClassNames.clear();
			s_Data->m_EntityClasses.clear();
			s_Data->m_ClassNames.clear();
			s_Data->m_Classes.clear();
		}
	}

	void ScriptEngine::ReloadAssembly()
	{
		// TODO: manage all .dll files in a folder
		MakeInstanceDataSnapshot();

		UnloadAssembly();
		LoadAssembly();

		Utils::PrintAssemblyTypes(s_Data->m_CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->m_ClientAssembly);
		LoadClassesFromAssembly(s_Data->m_CoreAssembly);
		LoadClassesFromAssembly(s_Data->m_ClientAssembly);

		LoadInternalClasses();

		// Recreate base Entity ScpritClass because CoreAssembly changed
		s_Data->m_EntityClass = ScriptClass("Stimpi", "Entity", s_Data->m_CoreAssembly);

		ScriptGlue::RegisterFucntions();
		ScriptGlue::RegisterComponents();

		CreateScriptInstances();

	}

	void ScriptEngine::ReloadAssetAssembly(const std::filesystem::path& assetPath)
	{
		auto relativePath = std::filesystem::relative(assetPath, ResourceManager::GetAssetsPath());
		std::string filenameStr = relativePath.filename().string();
		auto& assetAssembly = s_Data->m_AssetAssemblies[filenameStr];
		
		assetAssembly->m_Assembly = LoadMonoAssembly(assetPath.string());
		assetAssembly->m_AssemblyImage = mono_assembly_get_image(assetAssembly->m_Assembly);

		LoadClassesFromAssetAssembly(assetAssembly.get());
	}

	void ScriptEngine::MakeInstanceDataSnapshot()
	{
		//TODO: visit all scenes and save data
		if (s_Data->m_Scene)
		{
			for (auto& entity : s_Data->m_Scene->m_Entities)
			{
				if (entity.HasComponent<ScriptComponent>())
				{
					auto& scriptComponent = entity.GetComponent<ScriptComponent>();
					scriptComponent.MakeFieldDataSnapshot();
				}
			}
		}
	}

	void ScriptEngine::LoadClassesFromAssembly(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = GetClassInAssembly(s_Data->m_CoreAssembly, "Stimpi", "Entity"); //TODO: move this

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

			if (monoClass)
			{
				bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
				if (isEntity)
				{
					s_Data->m_ScriptClassNames.push_back(fullName);
					s_Data->m_EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name, assembly);
				}
			}
		}
	}

	// TODO: rework this as it is more specialized for only loading single class
	void ScriptEngine::LoadClassesFromAssetAssembly(AssetAssembly* assetAssembly)
	{
		MonoImage* image = mono_assembly_get_image(assetAssembly->m_Assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
		MonoClass* entityClass = GetClassInAssembly(s_Data->m_CoreAssembly, "Stimpi", "Entity"); //TODO: move this

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

			MonoClass* monoClass = GetClassInAssembly(assetAssembly->m_Assembly, nameSpace, name);
			if (entityClass == monoClass)
				continue;

			if (monoClass)
			{
				bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
				if (isEntity)
				{
					if (HasScriptClass(fullName))
					{
						ST_CORE_INFO("Class {} already exists in Scripts collection", fullName);
					}
					else
					{
						s_Data->m_ScriptClassNames.push_back(fullName);
					}
					assetAssembly->m_ScriptClassName = fullName;
					assetAssembly->m_EntityClass = std::make_shared<ScriptClass>(nameSpace, name, assetAssembly->m_Assembly);
					s_Data->m_EntityClasses[fullName] = assetAssembly->m_EntityClass;
				}
			}
		}
	}

	void ScriptEngine::LoadCustomClassesFromAssembly(MonoAssembly* assembly, const ClassLoadingDetails& classDetails)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

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
			if (monoClass)
			{
				for (auto& loadClass : classDetails)
				{
					if ((loadClass.m_NamespaceName == nameSpace) && (loadClass.m_ClassName == name))
					{
						if (s_Data->m_Classes.find(fullName) == s_Data->m_Classes.end())
						{
							s_Data->m_ClassNames.push_back(fullName);
							s_Data->m_Classes[fullName] = std::make_shared<ScriptClass>(nameSpace, name, assembly);
						}
					}
				}
			}
		}
	}

	void ScriptEngine::LoadInternalClasses()
	{
		ScriptEngine::LoadCustomClassesFromCoreAssembly({{ s_CoreNamespace, s_AttributeLookup }});
		auto scriptClass = ScriptEngine::GetClassByClassIdentifier({ s_CoreNamespace, s_AttributeLookup });
		s_Data->m_AttributeLookup = std::make_shared<ScriptInstance>(scriptClass);
	}

	void ScriptEngine::LoadCustomClassesFromCoreAssembly(const ClassLoadingDetails& classDetails)
	{
		LoadCustomClassesFromAssembly(s_Data->m_CoreAssembly, classDetails);
	}


	void ScriptEngine::LoadCustomClassesFromClientAssembly(const ClassLoadingDetails& classDetails)
	{
		LoadCustomClassesFromAssembly(s_Data->m_ClientAssembly, classDetails);
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


	std::shared_ptr<Stimpi::ScriptClass> ScriptEngine::GetClassByName(const std::string& className)
	{
		if (s_Data->m_Classes.find(className) != s_Data->m_Classes.end())
		{
			return s_Data->m_Classes.find(className)->second;
		}
		else
		{
			return nullptr;
		}
	}

	std::shared_ptr<ScriptClass> ScriptEngine::GetClassByClassIdentifier(const ClassIdentifier& identifier)
	{
		std::string fullName = fmt::format("{}.{}", identifier.m_NamespaceName, identifier.m_ClassName);
		return GetClassByName(fullName);
	}

	std::unordered_map<std::string, std::shared_ptr<Stimpi::ScriptClass>> ScriptEngine::GetEntityClasses()
	{
		return s_Data->m_EntityClasses;
	}

	std::shared_ptr<Stimpi::ScriptClass> ScriptEngine::GetScriptClassFromCoreAssembly(const std::string& className)
	{
		return std::make_shared<ScriptClass>("Stimpi", className, s_Data->m_CoreAssembly);
	}

	void ScriptEngine::InitMono()
	{
		mono_set_assemblies_path(MONO_LIB_PATH);

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
			ST_CORE_WARN("GetClassInAssembly error loading class {}.{}", namespaceName, className);
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

	std::filesystem::path ScriptEngine::GetCoreScriptPath()
	{
		return ResourceManager::GetScriptsPath() / s_CoreScriptName;
	}

	std::filesystem::path ScriptEngine::GetClientScriptPath()
	{
		return ResourceManager::GetScriptsPath() / s_ClientScriptName;
	}

	/**
	 * Remove all script instances marked for removal after Update step
	 * NOTE: C# destroy entity marks entities for deferred removal,
	 *	  so we can safely assume all Instances are in Unordered list
	 */
	void ScriptEngine::CleanUpRemovedInstances()
	{
		if (!s_Data->m_EntitiesToRemove.empty())
		{
			for (auto entity : s_Data->m_EntitiesToRemove)
			{
				s_Data->m_UnorderedEntityInstances.erase(entity);
				s_Data->m_Scene->RemoveEntity((entt::entity)entity);
			}
		}

		s_Data->m_EntitiesToRemove.clear();
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
	*  Called when reloading assembly
	*/
	void ScriptEngine::CreateScriptInstances()
	{
		s_Data->m_EntityInstances.clear();

		auto& entities = s_Data->m_Scene->m_Entities;
		for (Entity entity : entities)
		{
			if (entity.HasComponent<ScriptComponent>())
			{
				auto& scriptComponent = entity.GetComponent<ScriptComponent>();
				auto scriptInstance = CreateScriptInstance(scriptComponent.m_ScriptName, entity);
				scriptComponent.m_ScriptInstance = scriptInstance;
				// Reload and populate script instance field data
				scriptComponent.RestoreFiledData();
			}
		}
	}

	void ScriptEngine::ClearScriptInstances()
	{
		s_Data->m_EntityInstances.clear();
	}

	void ScriptEngine::OnScenePlay()
	{
		// Perform ordering before any other Script invocations
		InstanceUpdateOrderSorting();

		for (auto& element : s_Data->m_EntityInstances)
		{
			auto& instance = element.second;
			instance->InvokeOnCreate();
		}
	}

	void ScriptEngine::OnSceneUpdate(Timestep ts)
	{
		Clock::Begin();

		// New, somewhat ordered invocation
		for (auto& instance : s_Data->m_PriorityEntityInstances)
		{
			instance->InvokeOnUpdate(ts);
		}
		for (auto& element : s_Data->m_UnorderedEntityInstances)
		{
			auto& instance = element.second;
			instance->InvokeOnUpdate(ts);
		}

		CleanUpRemovedInstances();

		Statistics::SetScriptingTime(Clock::Stop());
	}

	void ScriptEngine::OnSceneStop()
	{
		s_Data->m_EntityInstances.clear();
		if (s_Data->m_DeferreAsemblyReload)
		{
			// TODO: avoid reloading assembly
			ReloadAssembly();
			s_Data->m_DeferreAsemblyReload = false;
		}

		// Clear other Instance collections
		s_Data->m_UnorderedEntityInstances.clear();
		s_Data->m_PriorityEntityInstances.clear();
	}


	void ScriptEngine::InstanceUpdateOrderSorting()
	{
		// TODO: FIXME: Configurable ordering by cy Class types
		for (auto& item : s_Data->m_EntityInstances)
		{
			auto entity = item.first;
			auto& instance = item.second;
			uint32_t order;
			if (instance->GetScriptOrderAttributeValue(&order))
			{
				ST_CORE_INFO("Instace of {} with ScriptOrder {}", instance->GetScriptClass()->GetFullName(), order);
				s_Data->m_PriorityEntityInstances.emplace_back(instance);
			}
			else
			{
				s_Data->m_UnorderedEntityInstances[entity] = instance;
			}
		}
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::OnScriptComponentAdd(const std::string& className, Entity entity)
	{
		// Remove previously used entry on the same entity
		OnScriptComponentRemove(entity);

		auto classInstance = CreateScriptInstance(className, entity);

		return classInstance;
	}

	void ScriptEngine::OnScriptComponentRemove(Entity entity)
	{
		if (s_Data->m_EntityInstances.find(entity) != s_Data->m_EntityInstances.end())
		{
			s_Data->m_EntityInstances.erase(entity);
		}

		// Check Instances created form C# scripts in runtime
		if (s_Data->m_UnorderedEntityInstances.find(entity) != s_Data->m_UnorderedEntityInstances.end())
		{
			s_Data->m_UnorderedEntityInstances.erase(entity);
		}
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::CreateScriptInstance(const std::string& className, Entity entity)
	{
		auto scriptClass = GetScriptClassByName(className);
		if (scriptClass == nullptr)
		{
			ST_CORE_ERROR("CreateScriptInstance failed, invalid className: {}", className);
			return nullptr;
		}

		auto classInstance = std::make_shared<ScriptInstance>(scriptClass, entity);
		if (s_Data->m_Scene)
		{
			// When Scene is STOPPED we are using main m_EntityInstances collection set
			if (s_Data->m_Scene->GetRuntimeState() == RuntimeState::STOPPED)
			{
				if (classInstance)
					s_Data->m_EntityInstances[entity] = classInstance;
			}

			// When in running mode, invoke onCreate on the new instance
			// Temp: Also add instances to m_UnorderedEntityInstances set
			if (s_Data->m_Scene->GetRuntimeState() != RuntimeState::STOPPED)
			{
				if (classInstance)
					s_Data->m_UnorderedEntityInstances[entity] = classInstance;

				classInstance->InvokeOnCreate();
			}
		}
		else
		{
			// We are here when the Scene is under deserialization (loaded)
			if (classInstance)
				s_Data->m_EntityInstances[entity] = classInstance;
		}

		return classInstance;
	}


	std::shared_ptr<ScriptInstance> ScriptEngine::CreateScriptInstance(const std::string& className)
	{
		auto scriptClass = GetScriptClassByName(className);
		if (scriptClass == nullptr)
			return nullptr;

		return std::make_shared<ScriptInstance>(scriptClass);
	}

	std::shared_ptr<Stimpi::ScriptInstance> ScriptEngine::GetScriptInstance(Entity entity)
	{
		if (s_Data->m_EntityInstances.find(entity) != s_Data->m_EntityInstances.end())
		{
			return s_Data->m_EntityInstances.at(entity);
		}
		else
		{
			if (s_Data->m_UnorderedEntityInstances.find(entity) != s_Data->m_UnorderedEntityInstances.end())
			{
				return s_Data->m_UnorderedEntityInstances.at(entity);
			}
		}
		return nullptr;
	}

	MonoObject* ScriptEngine::GetManagedInstance(uint32_t entityID)
	{
		if (s_Data->m_EntityInstances.find(entityID) != s_Data->m_EntityInstances.end())
		{
			auto& instance = s_Data->m_EntityInstances.at(entityID);
			return instance->GetMonoInstance();
		}
		else
		{
			if (s_Data->m_UnorderedEntityInstances.find(entityID) != s_Data->m_UnorderedEntityInstances.end())
			{
				auto& instance = s_Data->m_UnorderedEntityInstances.at(entityID);
				return instance->GetMonoInstance();
			}
		}
		return nullptr;
	}

	// Only called when Scene is not in STOPPED state (from C#)
	void ScriptEngine::RemoveEntityScriptInstance(uint32_t entityID)
	{
		if (s_Data->m_UnorderedEntityInstances.find(entityID) != s_Data->m_UnorderedEntityInstances.end())
		{
			s_Data->m_EntitiesToRemove.push_back(entityID);
		}
	}

	std::string ScriptEngine::GetFieldName(MonoClassField* field)
	{
		return mono_field_get_name(field);
	}

	std::vector<std::string>& ScriptEngine::GetScriptClassNames()
	{
		return s_Data->m_ScriptClassNames;
	}


	MonoReflectionType* ScriptEngine::GetMonoReflectionTypeByName(std::string typeName)
	{
		MonoType* monoType = mono_reflection_type_from_name(typeName.data(), ScriptEngine::GetCoreAssemblyImage());
		if (monoType == nullptr)
			monoType = mono_reflection_type_from_name(typeName.data(), s_Data->m_ClientAssemblyImage);
		if (monoType)
			return mono_type_get_object(ScriptEngine::GetAppDomain(), monoType);

		return nullptr;
	}

	MonoReflectionType* ScriptEngine::GetMonoReflectionType(MonoType* type)
	{
		if (type)
			return mono_type_get_object(ScriptEngine::GetAppDomain(), type);

		return nullptr;
	}

	MonoString* ScriptEngine::CreateMonoString(const std::string& str)
	{
		return mono_string_new(ScriptEngine::GetAppDomain(), str.c_str());
	}

	uint64_t ScriptEngine::GetGCUsedSize()
	{
		return mono_gc_get_used_size();
	}


	uint64_t ScriptEngine::GetGCHeapSize()
	{
		return mono_gc_get_heap_size();
	}

	void ScriptEngine::ForceGCCollect()
	{
		mono_gc_collect(mono_gc_max_generation());
	}

	uint32_t ScriptEngine::GetGCHandleCount()
	{
		return s_GCHandleCount;
	}

	/* ======== ScriptClass ======== */

	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className, MonoAssembly* monoAssembly)
		: m_Namespace(namespaceName), m_Name(className)
	{
		m_Class = ScriptEngine::GetClassInAssembly(monoAssembly, namespaceName.c_str(), className.c_str());
		ST_CORE_ASSERT(!m_Class);

		LoadFields();
	}

	ScriptClass::ScriptClass(MonoClass* monoClass)
		: m_Class(monoClass)
	{
		ST_CORE_ASSERT(!m_Class);
		m_Namespace = mono_class_get_namespace(m_Class);
		m_Name = mono_class_get_name(m_Class);

		LoadFields();
	}

	MonoObject* ScriptClass::Instantiate()
	{
		ST_CORE_ASSERT(!m_Class);
		return ScriptEngine::InstantiateClass(m_Class);
	}

	MonoMethod* ScriptClass::GetMethod(const std::string& methodName, int parameterCount)
	{
		MonoMethod* monoMethod = mono_class_get_method_from_name(m_Class, methodName.c_str(), parameterCount);
		if (monoMethod == nullptr)
		{
			if (SCRIPTENGINE_DBG) ST_CORE_WARN("[ScriptClass: {}] Error finding method {}", m_Name,  methodName);
		}

		return monoMethod;
	}

	void* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception = nullptr;
		MonoObject* returnObj = nullptr;
		void* retVal = nullptr;
		returnObj = mono_runtime_invoke(method, instance, params, &exception);
		if (returnObj)
			retVal = mono_object_unbox(returnObj);

		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}

		return retVal;
	}

	MonoClassField* ScriptClass::GetMonoField(const std::string& fieldName)
	{
		return mono_class_get_field_from_name(m_Class, fieldName.c_str());
	}

	std::shared_ptr<ScriptProperty> ScriptClass::GetPropertyByName(const std::string& propName)
	{
		std::shared_ptr<ScriptProperty> retProperty = nullptr;
		MonoProperty* prop = mono_class_get_property_from_name(m_Class, propName.c_str());
		if (prop)
		{
			retProperty = std::make_shared<ScriptProperty>(prop);
		}
		return retProperty;
	}

	void ScriptClass::LoadFields()
	{
		m_Fields.clear();
		if (SCRIPTENGINE_DBG) ST_CORE_INFO("Class: {}", mono_class_get_name(m_Class));

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
				if (SCRIPTENGINE_DBG) ST_CORE_INFO("Field:  {}", mono_field_get_name(field));
				if (SCRIPTENGINE_DBG) ST_CORE_INFO("Field: type {}, accessability {}", ttype, Utils::GetFieldAccessibility(field));
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
		m_Instance = std::make_shared<ScriptObject>(m_ScriptClass->Instantiate());
		m_Constructor = s_Data->m_EntityClass.GetMethod(".ctor", 1);
		m_OnCreateMethod = m_ScriptClass->GetMethod("OnCreate", 0);
		m_OnUpdateMethod = m_ScriptClass->GetMethod("OnUpdate", 1);

		/* Physics methods */
		m_OnCollisionBegin = m_ScriptClass->GetMethod("OnCollisionBegin", 1);
		m_OnCollisionEnd = m_ScriptClass->GetMethod("OnCollisionEnd", 1);
		m_OnCollisionPreSolve = m_ScriptClass->GetMethod("OnCollisionPreSolve", 1);
		m_OnCollisionPostSolve = m_ScriptClass->GetMethod("OnCollisionPostSolve", 1);

		void* param = &entity;
		m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_Constructor, &param);
	}

	ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass)
		: m_ScriptClass(scriptClass), m_Entity({})
	{
		m_Instance = std::make_shared<ScriptObject>(m_ScriptClass->Instantiate());
		m_Constructor = m_ScriptClass->GetMethod(".ctor", 0);

		m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_Constructor, nullptr);
	}

	ScriptInstance::~ScriptInstance()
	{
		if (SCRIPTENGINE_DBG) ST_CORE_TRACE("Destroy ScriptInstance");
	}

	void ScriptInstance::InvokeOnCreate()
	{
		if (m_OnCreateMethod)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCreateMethod, nullptr);
	}

	void ScriptInstance::InvokeOnUpdate(float ts)
	{
		void* param = &ts;
		if (m_OnUpdateMethod)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnUpdateMethod, &param);
	}

	void ScriptInstance::InvokeOnCollisionBegin(Collision collision)
	{
		void* param = &collision;
		if (m_OnCollisionBegin)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCollisionBegin, &param);
	}

	void ScriptInstance::InvokeOnCollisionEnd(Collision collision)
	{
		void* param = &collision;
		if (m_OnCollisionEnd)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCollisionEnd, &param);
	}

	bool ScriptInstance::InvokeOnCollisionPreSolve(Collision collision)
	{
		bool result = true;
		void* param = &collision;
		if (m_OnCollisionPreSolve)
		{
			void* res = m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCollisionPreSolve, &param);
			if (res)
				result = *(bool*)res;
		}

		return result;
	}

	void ScriptInstance::InvokeOnCollisionPostSolve(Collision collision)
	{
		void* param = &collision;
		if (m_OnCollisionPostSolve)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCollisionPostSolve, &param);
	}

	MonoObject* ScriptInstance::GetMonoInstance()
	{
		return m_Instance->GetMonoObject();
	}

	std::shared_ptr<Stimpi::ScriptObject>& ScriptInstance::GetInstance()
	{
		return m_Instance;
	}


	std::shared_ptr<Stimpi::ScriptClass>& ScriptInstance::GetScriptClass()
	{
		return m_ScriptClass;
	}

	std::unordered_map<std::string, std::shared_ptr<Stimpi::ScriptField>>& ScriptInstance::GetFields()
	{
		return m_Instance->GetFields();
	}


	std::shared_ptr<Stimpi::ScriptField> ScriptInstance::GetFieldByName(const std::string& fieldName)
	{
		return m_Instance->GetFieldByName(fieldName);
	}


	bool ScriptInstance::HasScriptOrderAttribute()
	{
		if (s_Data->m_AttributeLookup)
		{
			void* param;
			param = ScriptEngine::GetMonoReflectionType(m_Instance->GetMonoType());
			void* res = s_Data->m_AttributeLookup->InvokeMethod(s_HasScriptOrderAttribute, 1, &param);
			if (res)
			{
				return *(bool*)res;
			}
		}

		return false;
	}

	/**
	 * Reading [out]value of 0 means no ScriptOrder attribute defined. Returns false as well.
	 */
	bool ScriptInstance::GetScriptOrderAttributeValue(uint32_t* value)
	{
		*value = 0;
		if (s_Data->m_AttributeLookup)
		{
			void* params[2];
			params[0] = ScriptEngine::GetMonoReflectionType(m_Instance->GetMonoType());
			params[1] = value;
			void* res = s_Data->m_AttributeLookup->InvokeMethod(s_GetScriptOrderAttributeValue, 2, params);
			if (res)
			{
				return *(bool*)res;
			}
		}

		return false;
	}

	// This causes a small leak, looking up method all the time. Use only for testing stuff
	void* ScriptInstance::InvokeMethod(std::string methodName, int parameterCount, void** params)
	{
		MonoMethod* method = m_ScriptClass->GetMethod(methodName, parameterCount);
		if (method)
		{
			return m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), method, params);
		}
		return nullptr;
	}

	/* ======== ScriptObject ======== */

	ScriptObject::ScriptObject(MonoObject* obj)
		: m_MonoObject(obj), m_GCHandle(0)
	{
		MonoClass* klass = mono_object_get_class(m_MonoObject);
		m_MonoType = mono_class_get_type(klass);
		m_GCHandle = GetGCHandle(m_MonoObject, true);

		PopulateFieldsData();
	}


	ScriptObject::ScriptObject(std::string typeName)
		: m_MonoObject(nullptr), m_GCHandle(0)
	{
		MonoType* monoType = mono_reflection_type_from_name((char*)typeName.c_str(), s_Data->m_CoreAssemblyImage);
		if (monoType)
		{
			m_MonoObject = mono_object_new(ScriptEngine::GetAppDomain(), mono_class_from_mono_type(monoType));
			m_GCHandle = GetGCHandle(m_MonoObject, true);
			mono_runtime_object_init(m_MonoObject);
			PopulateFieldsData();
		}
	}

	ScriptObject::~ScriptObject()
	{
		if (m_GCHandle != 0)
			FreeGCHandle(m_GCHandle);
	}

	void ScriptObject::GetFieldValue(const std::string& fieldName, void* data)
	{
		if (m_Fields.find(fieldName) != m_Fields.end())
		{
			m_Fields.at(fieldName)->GetValue(data);
		}
	}

	void ScriptObject::SetFieldValue(const std::string& fieldName, void* data)
	{
		if (m_Fields.find(fieldName) != m_Fields.end())
		{
			m_Fields.at(fieldName)->SetValue(data);
		}
	}

	std::shared_ptr<ScriptObject> ScriptObject::GetFieldAsObject(const std::string& fieldName, bool createNew)
	{
		std::shared_ptr<ScriptObject> retObject = nullptr;

		if (m_Fields.find(fieldName) != m_Fields.end())
		{
			auto& field = m_Fields.at(fieldName);
			void* fieldData = nullptr;
			GetFieldValue(fieldName, &fieldData);
			if (fieldData)
			{
				retObject = std::make_shared<ScriptObject>((MonoObject*)fieldData);
			}
			else if (createNew)
			{
				retObject = std::make_shared<ScriptObject>(field->GetFieldTypeName());
				SetFieldValue(fieldName, retObject->GetMonoObject());
			}
		}

		return retObject;
	}

	std::shared_ptr<ScriptObject> ScriptObject::GetParentPropertyAsObject(const std::string& propName, bool createNew)
	{
		std::shared_ptr<ScriptObject> retObject = nullptr;

		MonoClass* klass = mono_object_get_class(m_MonoObject);
		// Cast to the desired sublcassName type
		MonoClass* parent = mono_class_get_parent(klass);
		if (parent)
		{
			MonoProperty* prop = mono_class_get_property_from_name(parent, propName.c_str());
			if (prop)
			{
				MonoMethod* propGet = mono_property_get_get_method(prop);
				if (propGet)
				{
					MonoObject* result;
					result = mono_runtime_invoke(propGet, m_MonoObject, nullptr, nullptr);
					if (result)
					{
						retObject = std::make_shared<ScriptObject>(result);
					}
					else if (createNew)
					{
						retObject = std::make_shared<ScriptObject>(std::string("Stimpi.").append(propName));
						MonoMethod* propSet = mono_property_get_set_method(prop);
						if (propSet)
						{
							void* param = retObject->GetMonoObject();
							mono_runtime_invoke(propSet, m_MonoObject, &param, nullptr);
						}
						int val = 10;
						retObject->GetFieldValue("ID", &val);
						val++;
					}
				}
			}
		}
		

		return retObject;
	}

	/*
	1) get parent class
	2) get property
	3) get/set value
	*/

	std::shared_ptr<ScriptObject> ScriptObject::GetParent()
	{
		std::shared_ptr<ScriptObject> parentObj = nullptr;

		MonoClass* klass = mono_object_get_class(m_MonoObject);
		MonoClass* parentClass = mono_class_get_parent(klass);
		if (parentClass)
		{
			MonoObject* castObj = mono_object_castclass_mbyref(m_MonoObject, parentClass);
			parentObj = std::make_shared<ScriptObject>(castObj);
		}

		return parentObj;
	}

	std::shared_ptr<ScriptClass> ScriptObject::GetParentClass()
	{
		std::shared_ptr<ScriptClass> retClass = nullptr;

		MonoClass* klass = mono_object_get_class(m_MonoObject);
		MonoClass* parentClass = mono_class_get_parent(klass);
		if (parentClass)
		{
			retClass = std::make_shared<ScriptClass>(parentClass);
		}

		return retClass;
	}

	std::unordered_map<std::string, std::shared_ptr<ScriptField>>& ScriptObject::GetFields()
	{
		return m_Fields;
	}


	std::shared_ptr<Stimpi::ScriptField> ScriptObject::GetFieldByName(const std::string& fieldName)
	{
		if (m_Fields.find(fieldName) != m_Fields.end())
		{
			return m_Fields.at(fieldName);
		}

		return nullptr;
	}

	void ScriptObject::PopulateFieldsData()
	{
		MonoClass* klass;
		MonoClassField* field;
		MonoType* fieldType;
		void* iter = nullptr;

		klass = mono_object_get_class(m_MonoObject);
		while ((field = mono_class_get_fields(klass, &iter)) != nullptr)
		{
			fieldType = mono_field_get_type(field);

			uint8_t vis = Utils::GetFieldAccessibility(field);
			if (vis & (uint8_t)Accessibility::PUBLIC)
			{
				std::string fieldName = mono_field_get_name(field);
				m_Fields[fieldName] = std::make_shared<ScriptField>(this, field);
			}
		}

		// TODO: make a wrapper type for script properties
		/*MonoProperty* prop;
		iter = nullptr;
		while ((prop = mono_class_get_properties(klass, &iter)) != nullptr)
		{
			std::string propName = mono_property_get_name(prop);
			ST_INFO("--- property name: {}", propName);
		}*/

		/*MonoClass* nested;
		iter = nullptr;
		while ((nested = mono_class_get_nested_types(klass, &iter)) != nullptr)
		{
			std::string nestedName = mono_class_get_name(nested);
			ST_INFO("--- nested name: {}", nestedName);
		}*/

		/*MonoClass* iface;
		iter = nullptr;
		while ((iface = mono_class_get_interfaces(klass, &iter)) != nullptr)
		{
			std::string ifeceName = mono_class_get_name(iface);
			ST_INFO("--- iface name: {}", ifeceName);
		}*/

		MonoType* classType = mono_class_get_type(klass);
		MonoType* underType = mono_type_get_underlying_type(classType);
		std::string underTypeName = mono_type_get_name(underType);
		//ST_INFO("--- underType name: {}", underTypeName);

		MonoClass* parent = mono_class_get_parent(klass);
		if (parent)
		{
			MonoProperty* prop = mono_class_get_property_from_name(parent, "Entity");
			// TODO: Move to populateProperties method, check if property is initialized (created object)

			if (prop)
			{
				MonoMethod* propGet = mono_property_get_get_method(prop);
				if (propGet)
				{
					MonoObject* result;
					result = mono_runtime_invoke(propGet, m_MonoObject, nullptr, nullptr);
				}
			}
		}


		MonoImage* image = mono_assembly_get_image(s_Data->m_CoreAssembly);
		MonoClass* testClass = mono_class_from_name(image, "Stimpi", "Component");
		if (testClass != nullptr)
		{
			MonoObject* propValObj;


			MonoObject* castObj = mono_object_castclass_mbyref(m_MonoObject, testClass);
			if (castObj)
			{
				MonoClass* castClass = mono_object_get_class(castObj);
				std::string casObjName = mono_class_get_name(castClass);
				//ST_INFO("--- cast class name: {}", casObjName);
			}

			MonoProperty* sprop;
			iter = nullptr;
			while ((sprop = mono_class_get_properties(testClass, &iter)) != nullptr)
			{
				std::string spropName = mono_property_get_name(sprop);
				//ST_INFO("--- s property name: {}", spropName);
			}
		}

	}

	/* ======== ScriptField ======== */

	ScriptField::ScriptField(ScriptObject* parent, MonoClassField* monoField)
		: m_ParentObject(parent), m_MonoField(monoField)
	{
		MonoType* monoType = mono_field_get_type(monoField);
		uint32_t dataType = mono_type_get_type(monoType);
		m_Type = Utils::GetFieldTypeFromMonoType(dataType);
		m_Name = mono_field_get_name(monoField);
		char* tmpCStr = mono_type_get_name(monoType);
		m_FieldTypeName = std::string(tmpCStr);
		mono_free(tmpCStr);

		size_t pos = m_FieldTypeName.find_last_of('.');
		m_FieldTypeShortName = m_FieldTypeName.substr(pos + 1);
	}

	void ScriptField::GetValue(void* data)
	{
		mono_field_get_value(m_ParentObject->GetMonoObject(), m_MonoField, data);
	}


	void ScriptField::SetValue(void* data)
	{
		mono_field_set_value(m_ParentObject->GetMonoObject(), m_MonoField, data);
	}

	bool ScriptField::IsSerializable()
	{
		if (s_Data->m_AttributeLookup)
		{
			void* attrParam[2];
			attrParam[0] = ScriptEngine::GetMonoReflectionType(m_ParentObject->GetMonoType());
			attrParam[1] = ScriptEngine::CreateMonoString(m_Name);
			void* res = s_Data->m_AttributeLookup->InvokeMethod("s_HasSerializeFieldAttribute", 2, attrParam);
			if (res)
			{
				return *(bool*)res;
			}
		}

		return false;
	}

	/* ======== ScriptProperty ======== */

	ScriptProperty::ScriptProperty(MonoProperty* monoProp)
		: m_MonoProperty(monoProp)
	{
		m_Name = mono_property_get_name(m_MonoProperty);

		m_GetMethod = mono_property_get_get_method(m_MonoProperty);
		m_SetMethod = mono_property_get_set_method(m_MonoProperty);
	}

	std::shared_ptr<ScriptObject> ScriptProperty::GetData(ScriptObject* ownerObject)
	{
		std::shared_ptr<ScriptObject> retObject = nullptr;

		if (m_GetMethod && ownerObject)
		{
			MonoObject* result;
			result = mono_runtime_invoke(m_GetMethod, ownerObject->GetMonoObject(), nullptr, nullptr);
			if (result)
				retObject = std::make_shared<ScriptObject>(result);
		}

		return retObject;
	}

	void ScriptProperty::SetData(ScriptObject* ownerObject, ScriptObject* dataObject)
	{
		if (m_SetMethod && ownerObject)
		{
			void* param = (void*)dataObject->GetMonoObject();
			mono_runtime_invoke(m_SetMethod, ownerObject->GetMonoObject(), &param, nullptr);
		}
	}

}