#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Physics/Physics.h"

extern "C"
{
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoMethod MonoMethod; 
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoClassField MonoClassField;
	typedef struct _MonoType MonoType;
	typedef struct _MonoProperty MonoProperty;
}

namespace Stimpi
{
	class ScriptClass;
	class ScriptInstance; 
	class ScriptField;

	struct ClassIdentifier
	{
		std::string m_NamespaceName;
		std::string m_ClassName;

		ClassIdentifier(const std::string& namespaceName, const std::string& className)
			: m_NamespaceName(namespaceName), m_ClassName(className)
		{}
	};

	struct ClassLoadingDetails
	{
		std::vector<ClassIdentifier> m_ClassIdentifiers;

		ClassLoadingDetails(std::initializer_list<ClassIdentifier> list)
		{
			for (auto item : list)
			{
				m_ClassIdentifiers.push_back(item);
			}
		}

		std::vector<ClassIdentifier>::iterator begin() { return m_ClassIdentifiers.begin(); }
		std::vector<ClassIdentifier>::iterator end() { return m_ClassIdentifiers.end(); }
	};

	struct AssetAssembly;

	class ST_API ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static void LoadAssembly();
		static void LoadAssetAsseblies();
		static void UnloadAssembly();
		static void LoadClassesFromAssembly(MonoAssembly* assembly);
		static void LoadClassesFromAssetAssembly(AssetAssembly* assetAssembly);

		static void LoadCustomClassesFromCoreAssembly(const ClassLoadingDetails& classDetails);
		static void LoadCustomClassesFromClientAssembly(const ClassLoadingDetails& classDetails);

		static void ReloadAssembly();
		static void ReloadAssetAssembly(const std::filesystem::path& assetPath);

		static bool HasScriptClass(const std::string& className);
		static std::shared_ptr<ScriptClass> GetScriptClassByName(const std::string& className);
		static std::shared_ptr<ScriptClass> GetClassByName(const std::string& className);
		static std::shared_ptr<ScriptClass> GetClassByClassIdentifier(const ClassIdentifier& identifier);
		static std::unordered_map<std::string, std::shared_ptr<ScriptClass>> GetEntityClasses();

		static MonoImage* GetCoreAssemblyImage();
		static MonoDomain* GetAppDomain();

		static void CreateScriptInstances();
		static void ClearScriptInstances();
		// Scene state updates
		static void OnScenePlay();
		static void OnSceneUpdate(Timestep ts);
		static void OnSceneStop();

		// Script component
		static void OnScriptComponentAdd(const std::string& className, Entity entity);
		static void OnScriptComponentRemove(Entity entity);

		// Instance functions
		static std::shared_ptr<ScriptInstance> CreateScriptInstance(const std::string& className, Entity entity);
		static std::shared_ptr<ScriptInstance> CreateScriptInstance(const std::string& className);
		static std::shared_ptr<ScriptInstance> GetScriptInstance(Entity entity);
		static MonoObject* GetManagedInstance(uint32_t entityID);

		// Field functions
		static std::string GetFieldName(MonoClassField* field);

		// UI Stuff
		static std::vector<std::string>& GetScriptClassNames();

		// Debugging
		static uint64_t GetGCUsedSize();
		static uint64_t GetGCHeapSize();
		static void ForceGCCollect();

	private:
		static void InitMono();
		static void ShutdownMono();

		static void LoadCustomClassesFromAssembly(MonoAssembly* assembly, const ClassLoadingDetails& classDetails);

		static MonoAssembly* LoadMonoAssembly(const std::string& assemblyPath);
		static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
		static MonoObject* InstantiateClass(MonoClass* monoClass);

		static std::filesystem::path GetCoreScriptPath();
		static std::filesystem::path GetClientScriptPath();

		friend ScriptClass;
	};

	class ScriptClass
	{
	public:
		ScriptClass() = default;
		ScriptClass(const std::string& namespaceName, const std::string& className, MonoAssembly* monoAssembly);

		MonoObject* Instantiate();

		MonoMethod* GetMethod(const std::string& methodName, int parameterCount);
		MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params);

		MonoClassField* GetField(const std::string& fieldName);
		std::vector<MonoClassField*>& GetAllFields() { return m_Fields; }
	private:
		void LoadFields();
		void LoadProperties();

		std::string m_Namespace;
		std::string m_Name;

		MonoClass* m_Class = nullptr;
		std::vector<MonoClassField*> m_Fields;
	};

	class ST_API ScriptInstance
	{
	public:
		ScriptInstance() = default;
		ScriptInstance(std::shared_ptr<ScriptClass> scriptClass);	// Create "empty" Instance
		ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity);
		~ScriptInstance();

		void InvokeOnCreate();
		void InvokeOnUpdate(float ts);

		/* Physics methods */
		void InvokeOnCollisionBegin(Collision collision);
		void InvokeOnCollisionEnd(Collision collision);
		void InvokeOnCollisionPreSolve(Collision collision);
		void InvokeOnCollisionPostSolve(Collision collision);

		MonoObject* GetInstance() { return m_Instance; }
		std::vector<std::shared_ptr<ScriptField>>& GetFields() { return m_ScriptFields; }

		std::shared_ptr<ScriptField> GetScriptFieldFromMonoField(MonoClassField* field);

		// Custom method invocation - for non-Entity scripts
		void InvokeMethod(std::string methodName, int parameterCount = 0, void** params = nullptr);

	private:
		std::shared_ptr<ScriptClass> m_ScriptClass;
		Entity m_Entity;

		MonoObject* m_Instance = nullptr;
		MonoMethod* m_Constructor = nullptr;
		MonoMethod* m_OnCreateMethod = nullptr;
		MonoMethod* m_OnUpdateMethod = nullptr;

		/* Physics methods */
		MonoMethod* m_OnCollisionBegin = nullptr;
		MonoMethod* m_OnCollisionEnd = nullptr;
		MonoMethod* m_OnCollisionPreSolve = nullptr;
		MonoMethod* m_OnCollisionPostSolve = nullptr;

		/* Manage a collection of ScriptField objects that are wrappers around C# Field */
		std::vector<std::shared_ptr<ScriptField>> m_ScriptFields;
	};

	class ST_API ScriptField
	{
	public:
		ScriptField(ScriptInstance* instance, MonoClassField* field);

		MonoClassField* GetMonoField() { return m_MonoField; }
		void ReadFieldValue(void* value);
		void SetFieldValue(void* value);
	private:
		ScriptInstance* m_Instance;
		MonoClassField* m_MonoField;
		void* m_Data;
	};
}