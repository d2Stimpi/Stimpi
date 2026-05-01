#include "stpch.h"

#include "Gui/Event/EditorEventManager.h"

namespace Stimpi
{

	std::vector<EditorEventHandler*> s_EditorEventHandlers;

	void EditorEventManager::AddEditorEventHandler(EditorEventHandler* handler)
	{
		if (handler)
		{
			s_EditorEventHandlers.push_back(handler);
		}
	}

	void EditorEventManager::RemoveEditorEventHandler(EditorEventHandler* handler)
	{
		if (handler)
		{
			s_EditorEventHandlers.erase(std::remove(s_EditorEventHandlers.begin(), s_EditorEventHandlers.end(), handler),
				s_EditorEventHandlers.end());
		}
	}

	bool EditorEventManager::HandleEditorEvent(EditorEvent* event)
	{
		bool handled = false;

		for (auto handler : s_EditorEventHandlers)
		{
			handled = handler->OnEditorEvent(event);
			if (handled == true)
				break; // Consume the event
		}

		return handled;
	}

}