#pragma once

#include "Stimpi/Core/Core.h"
#include <string>

// TODO: abstract types to isolate system specifics
#include "Windows.h"

#define STSHELL_USER_EVENT		WM_USER + 200

namespace Stimpi
{
	class ST_API FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

		static std::string OpenFolder();
	};

	struct ST_API ShellEvetData
	{
		long m_Event = 0;
		std::string m_FilePath;
		std::string m_NewFilePath;

		ShellEvetData() = default;
		ShellEvetData(long event, const std::string& filePath, const std::string& newFilePath)
			: m_Event(event), m_FilePath(filePath), m_NewFilePath(newFilePath)
		{
		}
	};

	class ST_API ShellUtils
	{
	public:
		static bool ChangeNotifyRegister(const char* path, unsigned long* handle);
		static bool ChangeNotifyDegister(unsigned long handle);

		static void ProcessEvent(WPARAM wParam, LPARAM lParam, ShellEvetData* shEvent);
	};
}