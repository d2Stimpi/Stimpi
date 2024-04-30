#include "stpch.h"
#include "Stimpi/Utils/FileWatcher.h"
#include "Stimpi/Utils/PlatformUtils.h"

namespace Stimpi
{
	struct FileWatcherData
	{
		std::vector<FileWatcherEntry> m_WatcherEntries;
	};

	static FileWatcherData s_Data;

	void FileWatcher::AddWatcher(std::filesystem::path watchPath, FileWatchListener watcher)
	{
		unsigned long handle;

		if (watcher != nullptr)
		{
			ShellUtils::ChangeNotifyRegister(watchPath.string().c_str(), &handle);

			FileWatcherEntry entry = { watcher, handle, watchPath };
			s_Data.m_WatcherEntries.push_back(entry);
		}
	}

	void FileWatcher::RemoveWatcher(std::filesystem::path watchPath)
	{
		if (auto it = std::find_if(s_Data.m_WatcherEntries.begin(), s_Data.m_WatcherEntries.end(),
			[=](FileWatcherEntry entry) -> bool {
				return entry.m_WatchPath == watchPath;
			}); it != s_Data.m_WatcherEntries.end())
		{
			ST_CORE_INFO("FileWatcher - Remove watcher for path: {}", (*it).m_WatchPath);
			ShellUtils::ChangeNotifyDegister((*it).m_Handle);
			
			s_Data.m_WatcherEntries.erase(it);
		}
	}

	void FileWatcher::Clear()
	{
		for (auto entry : s_Data.m_WatcherEntries)
		{
			RemoveWatcher(entry.m_WatchPath);
		}
	}

	void FileWatcher::ProcessShellEvent(std::shared_ptr<SystemShellEvent> event)
	{
		std::string path = event->GetFilePath();
		
		for (auto entry : s_Data.m_WatcherEntries)
		{
			if (path.find(entry.m_WatchPath.string()) != std::string::npos)
			{
				entry.m_Listener(event.get());
			}

			if (path == entry.m_WatchPath.string())
			{
				entry.m_Listener(event.get());
			}
		}
	}

}