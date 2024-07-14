#include "stpch.h"
#include "Gui/Panels/ScriptFieldFragment.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/SceneManager.h"

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

	static std::string EntityFieldTypeFragment(ScriptObject* ownerObj, ScriptField* field)
	{
		uint32_t fieldData = 0;
		auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), false);
		if (entityObj)
		{
			entityObj->GetFieldValue("ID", &fieldData);
		}

		auto activeScene = SceneManager::Instance()->GetActiveScene();
		Entity entity = activeScene->GetEntityByHandle((entt::entity)fieldData);
		std::string tagStr = "None";
		if (entity)
		{
			TagComponent tag = entity.GetComponent<TagComponent>();
			tagStr = tag.m_Tag;
		}

		return tagStr;
	}

	static void EntityFieldTypePayload(ScriptObject* ownerObj, ScriptField* field)
	{
		UIPayload::BeginTarget(PAYLOAD_DATA_TYPE_ENTITY, [&ownerObj, &field](void* data, uint32_t size)
			{
				auto entityObj = ownerObj->GetFieldAsObject(field->GetName(), true);
				entityObj->SetFieldValue("ID", data);
			});
	}

	/* ======== ScriptFieldFragment generic functions ======== */

	void ScriptFieldFragment::RegisterScriptFieldFunctions()
	{
		ST_REGISTER_FIELD_FRAGMENT_TYPE("Stimpi.Entity", EntityFieldTypeFragment);
		ST_REGISTER_FIELD_PAYLOAD_TYPE("Stimpi.Entity", EntityFieldTypePayload);
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
		std::string label = fmt::format("##ScriptClassField_{}_{}", field->GetName(), (uint32_t)selectedEntity);
		std::string text = fmt::format("{} ({})", tagStr, fieldTypeShortName);
		ImGui::InputText(label.c_str(), text.data(), text.length(), ImGuiInputTextFlags_ReadOnly);

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
		if (s_ScriptFieldTypePayloadFunctions.find(fieldTypeName) != s_ScriptFieldTypePayloadFunctions.end())
		{
			return s_ScriptFieldTypePayloadFunctions.at(fieldTypeName)(ownerObj, field);
		}

	}

}