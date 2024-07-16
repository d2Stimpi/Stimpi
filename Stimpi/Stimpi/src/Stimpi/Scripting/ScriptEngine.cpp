#include "stpch.h"
#include "Stimpi/Scripting/ScriptEngine.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Event.h"
#include "Stimpi/Utils/SystemUtils.h"
#include "Stimpi/Utils/FileWatcher.h"

#include "Stimpi/Scene/ResourceManager.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scene/Component.h"

#include "Stimpi/Scripting/ScriptGlue.h"

#include "mono/jit/jit.h"
#include "mono/metadata/appdomain.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/attrdefs.h"
#include "mono/metadata/mono-gc.h"

/** TODO: fix issues:
 *  - Component adds a Script -> Script needs to be instantiated and so. Atm it won't be "active" before scene is reloaded
 *  - Editor crash when adding new script and attemtping to read Fields; Related to above issue.
 * 
 *  Features
 *  - Find a better way to handle Field & Property data, keeping a local value, type handling...
 */

#define SCRIPTENGINE_DBG	(true)

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

		/* Scene data - Class Instances per entity */
		std::shared_ptr<Scene> m_Scene;
		std::unordered_map<uint32_t, std::shared_ptr<ScriptInstance>> m_EntityInstances;

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
	};

	static ScriptEngineData* s_Data;
	static std::string s_CoreScriptName = "Stimpi-ScriptCore.dll";

	static std::string s_ClientScriptName = "Sandbox-Script.dll";

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

		InitMono();
		LoadAssembly();

		LoadAssetAsseblies();

		Utils::PrintAssemblyTypes(s_Data->m_CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->m_ClientAssembly);
		LoadClassesFromAssembly(s_Data->m_CoreAssembly);
		LoadClassesFromAssembly(s_Data->m_ClientAssembly);

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

			MonoException* exc = NULL;
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

		UnloadAssembly();
		LoadAssembly();

		Utils::PrintAssemblyTypes(s_Data->m_CoreAssembly);
		Utils::PrintAssemblyTypes(s_Data->m_ClientAssembly);
		LoadClassesFromAssembly(s_Data->m_CoreAssembly);
		LoadClassesFromAssembly(s_Data->m_ClientAssembly);

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
				auto loadDetails = classDetails;
				for (auto loadClass : loadDetails)
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

	std::filesystem::path ScriptEngine::GetCoreScriptPath()
	{
		return ResourceManager::GetScriptsPath() / s_CoreScriptName;
	}

	std::filesystem::path ScriptEngine::GetClientScriptPath()
	{
		return ResourceManager::GetScriptsPath() / s_ClientScriptName;
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

		auto entities = s_Data->m_Scene->m_Entities;
		for (Entity entity : entities)
		{
			if (entity.HasComponent<ScriptComponent>())
			{
				auto scriptComponent = entity.GetComponent<ScriptComponent>();
				auto scriptInstance = CreateScriptInstance(scriptComponent.m_ScriptName, entity);
				s_Data->m_EntityInstances[entity] = scriptInstance;
			}
		}
	}

	void ScriptEngine::ClearScriptInstances()
	{
		s_Data->m_EntityInstances.clear();
	}

	void ScriptEngine::OnScenePlay()
	{
		for (auto& element : s_Data->m_EntityInstances)
		{
			auto& instance = element.second;
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
		if (s_Data->m_DeferreAsemblyReload)
		{
			// TODO: avoid reloading assembly
			ReloadAssembly();
			s_Data->m_DeferreAsemblyReload = false;
		}
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::OnScriptComponentAdd(const std::string& className, Entity entity)
	{
		// Remove previously used entry on the same entity
		OnScriptComponentRemove(entity);

		auto classInstance = CreateScriptInstance(className, entity);
		if (classInstance)
			s_Data->m_EntityInstances[entity] = classInstance;

		return classInstance;
	}

	void ScriptEngine::OnScriptComponentRemove(Entity entity)
	{
		if (s_Data->m_EntityInstances.find(entity) != s_Data->m_EntityInstances.end())
		{
			s_Data->m_EntityInstances.erase(entity);
		}
	}

	std::shared_ptr<ScriptInstance> ScriptEngine::CreateScriptInstance(const std::string& className, Entity entity)
	{
		auto scriptClass = GetScriptClassByName(className);
		if (scriptClass == nullptr)
			return nullptr;

		return std::make_shared<ScriptInstance>(scriptClass, entity);
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
			return nullptr;
		}
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
			return nullptr;
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

	/* ======== ScriptClass ======== */

	ScriptClass::ScriptClass(const std::string& namespaceName, const std::string& className, MonoAssembly* monoAssembly)
		: m_Namespace(namespaceName), m_Name(className)
	{
		m_Class = ScriptEngine::GetClassInAssembly(monoAssembly, namespaceName.c_str(), className.c_str());
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
			ST_CORE_WARN("[ScriptClass] Error finding method {}", methodName);
		}

		return monoMethod;
	}

	MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params)
	{
		MonoObject* exception;
		mono_runtime_invoke(method, instance, params, &exception);

		if (exception)
		{
			mono_print_unhandled_exception(exception);
		}

		return nullptr;
	}

	MonoClassField* ScriptClass::GetMonoField(const std::string& fieldName)
	{
		return mono_class_get_field_from_name(m_Class, fieldName.c_str());
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

	void ScriptInstance::InvokeOnCollisionPreSolve(Collision collision)
	{
		void* param = &collision;
		if (m_OnCollisionPreSolve)
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), m_OnCollisionPreSolve, &param);
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


	std::unordered_map<std::string, std::shared_ptr<Stimpi::ScriptField>>& ScriptInstance::GetFields()
	{
		return m_Instance->GetFields();
	}


	std::shared_ptr<Stimpi::ScriptField> ScriptInstance::GetFieldByName(const std::string& fieldName)
	{
		return m_Instance->GetFieldByName(fieldName);
	}

	// This causes a small leak, looking up method all the time. Use only for testing stuff
	void ScriptInstance::InvokeMethod(std::string methodName, int parameterCount, void** params)
	{
		MonoMethod* method = m_ScriptClass->GetMethod(methodName, parameterCount);
		if (method)
		{
			m_ScriptClass->InvokeMethod(m_Instance->GetMonoObject(), method, params);
		}
	}

	/* ======== ScriptObject ======== */

	ScriptObject::ScriptObject(MonoObject* obj)
		: m_MonoObject(obj)
	{
		PopulateFieldsData();
	}


	ScriptObject::ScriptObject(std::string typeName)
		: m_MonoObject(nullptr)
	{
		//TODO: check if const char* -> char* has no side effects
		MonoType* monoType = mono_reflection_type_from_name((char*)typeName.c_str(), s_Data->m_CoreAssemblyImage);
		if (monoType)
		{
			m_MonoObject = mono_object_new(ScriptEngine::GetAppDomain(), mono_class_from_mono_type(monoType));
			mono_runtime_object_init(m_MonoObject);
			PopulateFieldsData();
		}
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
	}

	/* ======== ScriptField ======== */

	ScriptField::ScriptField(ScriptObject* parent, MonoClassField* monoField)
		: m_ParentObject(parent), m_MonoField(monoField)
	{
		MonoType* monoType = mono_field_get_type(monoField);
		uint32_t dataType = mono_type_get_type(monoType);
		m_Type = Utils::GetFieldTypeFromMonoType(dataType);
		m_Name = mono_field_get_name(monoField);
		m_FieldTypeName = mono_type_get_name(monoType);

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

}