#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Event.h"

#include <functional>
#include <filesystem>

namespace Stimpi
{
	// Add ShellEvent - file and dir changes event
	using FileWatchListener = std::function<void(SystemShellEvent* event)>;

	struct ST_API FileWatcherEntry
	{
		FileWatchListener m_Listener = nullptr;
		unsigned long m_Handle = 0;
		std::filesystem::path m_WatchPath;

		FileWatcherEntry() = default;
		FileWatcherEntry(const FileWatcherEntry&) = default;
		FileWatcherEntry(FileWatchListener listener, unsigned long handle, std::filesystem::path path)
			: m_Listener(listener), m_Handle(handle), m_WatchPath(path)
		{}
	};

	// Weakness - multiple watchers on the same path might cause issues - no special handling
	class ST_API FileWatcher
	{
	public:
		/* When FileWatchListener is added start "Watching" and stop on removing it */
		static void AddWatcher(std::filesystem::path watchPath, FileWatchListener watcher);
		static void RemoveWatcher(std::filesystem::path watchPath);
		static void Clear();

		static void ProcessShellEvent(std::shared_ptr<SystemShellEvent> event);
	};

}