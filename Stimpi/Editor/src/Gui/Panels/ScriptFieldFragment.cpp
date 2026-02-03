#include "stpch.h"
#include "Gui/Panels/ScriptFieldFragment.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Scripting/ScriptGlueTypes.h"

#include "Gui/Components/Input.h"
#include "Gui/Components/UIPayload.h"
#include "Gui/Panels/SceneHierarchyWindow.h"

#define ST_ADD_FIELD_TYPE_UNPACK(FieldName, ReadFunc)				s_ScriptFieldUnpackFunctions[FieldName] = ReadFunc;
#define ST_ADD_FIELD_TYPE_PACK(FieldName, WriteFunc)				s_ScriptFieldPackFunctions[FieldName] = WriteFunc;

#define ST_REGISTER_FIELD_FRAGMENT_TYPE(FieldTypeName, Func)		s_ScriptFieldTypeFragmentFunctions[FieldTypeName] = Func;
#define ST_REGISTER_FIELD_PAYLOAD_TYPE(FieldTypeName, Func)			s_ScriptFieldTypePayloadFunctions[FieldTypeName] = Func;

namespace Stimpi
{
	// TODO: use these
	static std::unordered_map<std::string, std::function<void(void)>> s_ScriptFieldUnpackFunctions;
	static std::unordered_map<std::string, std::function<void(void)>> s_ScriptFieldPackFunctions;

	// returns a "tagStr" that can be shown in InputText box
	static std::unordered_map<std::string, std::function<std::string(ScriptObject*, ScriptField*)>> s_ScriptFieldTypeFragmentFunctions;
	static std::unordered_map<std::string, std::function<void(ScriptObject*, ScriptField*)>> s_ScriptFieldTypePayloadFunctions;

	// Entity field type

	static std::string EntityFieldTypeFragment(ScriptObject* ownerObj, ScriptField* field)
	{
		uint32_t fieldData = INVALID_ENTITY_HANDLE;
		AssetHandle prefabHandle = INVALID_ASSET_HANDLE;
		auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), false);
		if (entityObj)
		{
			entityObj->GetFieldValue("ID", &fieldData);
			entityObj->GetFieldValue("PrefabHandle", &prefabHandle);
		}

		auto activeScene = SceneManager::Instance()->GetActiveScene();
		Entity entity = activeScene->GetEntityByHandle((entt::entity)fieldData);
		std::string tagStr = "None";
		if (entity)
		{
			TagComponent tag = entity.GetComponent<TagComponent>();
			tagStr = tag.m_Tag;
		}
		else if (prefabHandle)
		{
			auto prefabAsset = AssetManager::GetAsset<Prefab>(prefabHandle);
			if (prefabAsset->GetType() == Prefab::GetTypeStatic())
			{
				tagStr = prefabAsset->GetName();
			}
		}

		return tagStr;
	}

	static void EntityFieldTypePayload(ScriptObject* ownerObj, ScriptField* field)
	{
		UIPayload::BeginTarget(PAYLOAD_DATA_TYPE_ENTITY, [&ownerObj, &field](void* data, uint32_t size)
			{
				AssetHandle handle = INVALID_ASSET_HANDLE;
				auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
				entityObj->SetFieldValue("ID", data);
				entityObj->SetFieldValue("PrefabHandle", &handle);
			});

		UIPayload::BeginTarget(PAYLOAD_PREFAB, [&ownerObj, &field](void* data, uint32_t size)
			{
				uint32_t handle = INVALID_ENTITY_HANDLE;
				auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
				entityObj->SetFieldValue("ID", &handle);
				entityObj->SetFieldValue("PrefabHandle", data);
			});
	}

	// Component field type

	static std::string ComponentFieldTypeFragment(ScriptObject* ownerObj, ScriptField* field)
	{
		uint32_t fieldData = INVALID_ENTITY_HANDLE;
		AssetHandle prefabHandle = INVALID_ASSET_HANDLE;
		std::shared_ptr<ScriptObject> entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
		std::shared_ptr<ScriptClass> parent = entityObj->GetParentClass();
		std::shared_ptr<ScriptProperty> property = parent->GetPropertyByName("Entity");
		std::shared_ptr<ScriptObject> propertyObj = property->GetData(entityObj.get());
		if (propertyObj)
		{
			propertyObj->GetFieldValue("ID", &fieldData);
			propertyObj->GetFieldValue("PrefabHandle", &prefabHandle);
		}

		auto activeScene = SceneManager::Instance()->GetActiveScene();
		Entity entity = activeScene->GetEntityByHandle((entt::entity)fieldData);
		std::string tagStr = "None";
		if (entity)
		{
			TagComponent tag = entity.GetComponent<TagComponent>();
			tagStr = tag.m_Tag;
		}
		else if (prefabHandle)
		{
			auto prefabAsset = AssetManager::GetAsset<Prefab>(prefabHandle);
			if (prefabAsset->GetType() == Prefab::GetTypeStatic())
			{
				tagStr = prefabAsset->GetName();
			}
		}

		return tagStr;
	}


#define HAS_COMPONENT_BY_TYPENAME_IFELSE(Entity, ComponentName, Variable)	\
		if (Variable == #ComponentName)						\
		{												\
			Entity.AddComponent<ComponentName>(ComponentName(Node[#ComponentName]));	\
		}

	static void ComponentFieldTypePayload(ScriptObject* ownerObj, ScriptField* field)
	{
		UIPayload::BeginTarget(PAYLOAD_DATA_TYPE_ENTITY, [&ownerObj, &field](void* data, uint32_t size)
			{
				AssetHandle handle = INVALID_ASSET_HANDLE;
				Entity entity = *(Entity*)data;
				std::string fieldTypeName = field->GetFieldTypeName();

				if (fieldTypeName == s_QuadComponentType)
				{
					if (!entity.HasComponent<QuadComponent>())
						return;
				}

				// Get field that was inspected for having required Component type
				auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);

				// Get parent from the Component type field
				std::shared_ptr<ScriptClass> parent = entityObj->GetParentClass();

				// Get the Entity data, defined as C# Property type
				std::shared_ptr<ScriptProperty> property = parent->GetPropertyByName("Entity");

				// Get Entity data from Component object
				std::shared_ptr<ScriptObject> propertyObj = property->GetData(entityObj.get());
				if (propertyObj == nullptr)
				{
					propertyObj = std::make_shared<ScriptObject>("Stimpi.Entity");
				}
				propertyObj->SetFieldValue("ID", data);
				propertyObj->SetFieldValue("PrefabHandle", &handle);
				property->SetData(entityObj.get(), propertyObj.get());
			});

		UIPayload::BeginTarget(PAYLOAD_PREFAB, [&ownerObj, &field](void* data, uint32_t size)
			{
				uint32_t handle = INVALID_ENTITY_HANDLE;
				std::string fieldTypeName = field->GetFieldTypeName();
				AssetHandle prefabHandle = *(AssetHandle*)data;
				auto prefabAsset = AssetManager::GetAsset<Prefab>(prefabHandle);

				if (prefabAsset->GetType() == Prefab::GetTypeStatic())
				{
					if (fieldTypeName == s_QuadComponentType)
					{
						if (!prefabAsset->HasComponent<QuadComponent>())
							return;
					}
				}

				auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
				std::shared_ptr<ScriptClass> parent = entityObj->GetParentClass();
				std::shared_ptr<ScriptProperty> property = parent->GetPropertyByName("Entity");
				std::shared_ptr<ScriptObject> propertyObj = property->GetData(entityObj.get());

				if (propertyObj == nullptr)
				{
					propertyObj = std::make_shared<ScriptObject>("Stimpi.Entity");
				}
				propertyObj->SetFieldValue("ID", &handle);
				propertyObj->SetFieldValue("PrefabHandle", data);
				property->SetData(entityObj.get(), propertyObj.get());

				AssetHandle assetH = INVALID_ASSET_HANDLE;
				propertyObj->GetFieldValue("PrefabHandle", &assetH);
				assetH;

			});
	}

	/* ======== ScriptFieldFragment generic functions ======== */

	void ScriptFieldFragment::RegisterScriptFieldFunctions()
	{
		// Entity
		ST_REGISTER_FIELD_FRAGMENT_TYPE(s_EntityType, EntityFieldTypeFragment);
		ST_REGISTER_FIELD_PAYLOAD_TYPE(s_EntityType, EntityFieldTypePayload);
		// QuadComponent
		ST_REGISTER_FIELD_FRAGMENT_TYPE(s_QuadComponentType, ComponentFieldTypeFragment);
		ST_REGISTER_FIELD_PAYLOAD_TYPE(s_QuadComponentType, ComponentFieldTypePayload);
		
	}

	bool ScriptFieldFragment::IsFieldTypeSupported(const std::string& typeName)
	{
		return s_ScriptFieldTypeFragmentFunctions.find(typeName) != s_ScriptFieldTypeFragmentFunctions.end();
	}

	void ScriptFieldFragment::ScriptFieldInput(ScriptObject* ownerObj, ScriptField* field)
	{
		if (ownerObj == nullptr || field == nullptr)
			ST_CORE_ASSERT("ScriptFieldFragment - invalid params");

		auto selectedEntity = SceneHierarchyWindow::GetSelectedEntity();
		std::string fieldTypeShortName = field->GetFieldTypeShortName();

		std::string tagStr = CallFragmentFunction(ownerObj, field);

		// Unique field label
		std::string label = fmt::format("{}##ScriptClassField_{}_{}", field->GetName(), field->GetName(), (uint32_t)selectedEntity);
		std::string text = fmt::format("{} ({})", tagStr, fieldTypeShortName);
		UI::Input::InputText(label.c_str(), text.data(), text.length(), ImGuiInputTextFlags_ReadOnly);

		HandlePayloadType(ownerObj, field);
	}

	std::string ScriptFieldFragment::CallFragmentFunction(ScriptObject* ownerObj, ScriptField* field)
	{
		std::string fieldTypeName = field->GetFieldTypeName();
		if (s_ScriptFieldTypeFragmentFunctions.find(fieldTypeName) != s_ScriptFieldTypeFragmentFunctions.end())
		{
			return s_ScriptFieldTypeFragmentFunctions.at(fieldTypeName)(ownerObj, field);
		}

		return "Unknown";
	}

	void ScriptFieldFragment::HandlePayloadType(ScriptObject* ownerObj, ScriptField* field)
	{
		std::string fieldTypeName = field->GetFieldTypeName();

		// First check if Prefab AssetHandle can be a valid target type
		/*std::shared_ptr<ScriptObject> entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
		std::shared_ptr<ScriptClass> parent = entityObj->GetParentClass();
		std::string fieldParentTypeName = parent->GetFullName();
		if (fieldTypeName == s_EntityType || fieldParentTypeName == s_ComponentType)
		{
			bool handled = HandlePrefabPayload(ownerObj, field);
			if (handled)
				return;
		}*/


		if (s_ScriptFieldTypePayloadFunctions.find(fieldTypeName) != s_ScriptFieldTypePayloadFunctions.end())
		{
			return s_ScriptFieldTypePayloadFunctions.at(fieldTypeName)(ownerObj, field);
		}

	}

}