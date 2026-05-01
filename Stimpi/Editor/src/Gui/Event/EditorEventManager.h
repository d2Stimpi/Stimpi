#pragma once

#include "Gui/Event/EditorEvent.h"

namespace Stimpi
{
	class EditorEventHandler
	{
		using EditorEventHandlerFunction = std::function<bool(EditorEvent*)>;

	public:
		EditorEventHandler(EditorEventHandlerFunction onEvent) : m_OnEvent(onEvent) {}
		bool OnEditorEvent(EditorEvent* e) { if (m_OnEvent) { return m_OnEvent(e); } else return false; }

	private:
		EditorEventHandlerFunction m_OnEvent{};
	};

	class EditorEventManager
	{
	public:
		static void AddEditorEventHandler(EditorEventHandler* handler);
		static void RemoveEditorEventHandler(EditorEventHandler* handler);

		static bool HandleEditorEvent(EditorEvent* event);
	};
}