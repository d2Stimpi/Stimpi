#include "stpch.h"
#include "Stimpi/Utils/PlatformUtils.h"

#include "Stimpi/Log.h"

#include "Windows.h"
#include <WinUser.h>
#include <ShlObj_core.h>
#include <ShObjIdl.h>
#include <commdlg.h>
#include <SDL.h>
#include <SDL_syswm.h>

namespace Stimpi
{
	static HWND GetWindowHandle()
	{
		SDL_Window* sdlWindow = SDL_GL_GetCurrentWindow();
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(sdlWindow, &wmInfo);

		return wmInfo.info.win.window;
	}

	std::string FileDialogs::OpenFile(const char* filter)
	{
		HWND hwnd = GetWindowHandle();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn , sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		HWND hwnd = GetWindowHandle();

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}

	std::string FileDialogs::OpenFolder()
	{
		HWND hwnd = GetWindowHandle();

		BROWSEINFO bfn;
		TCHAR szDir[260] = { 0 };
		ZeroMemory(&bfn, sizeof(BROWSEINFO));
		bfn.ulFlags = BIF_USENEWUI;
		bfn.lpszTitle = L"Caption";
		bfn.hwndOwner = hwnd;
		bfn.pszDisplayName = szDir;

		LPITEMIDLIST lpItem = SHBrowseForFolder(&bfn);
		if (lpItem != NULL)
		{
			CHAR szPath[260] = { 0 };
			SHGetPathFromIDListA(lpItem, szPath);
			return szPath;
		}

		return std::string();
	}

	bool ShellUtils::ChangeNotifyRegister(const char* path, unsigned long* handle)
	{
		HWND hwnd = GetWindowHandle();
		wchar_t wPath[MAX_PATH];
		PCWSTR pszPath;
		HRESULT hr = 0;

		mbstowcs(wPath, path, strlen(path) + 1);
		pszPath = wPath;

		IShellItem* psiItem;
		hr = SHCreateItemFromParsingName(pszPath, NULL, IID_PPV_ARGS(&psiItem));
		if (hr == ERROR_FILE_NOT_FOUND)
		{
			ST_CORE_ERROR("File {} not found", path);
			return false;
		}
		else if (!SUCCEEDED(hr))
		{
			ST_CORE_ERROR("SHCreateItemFromParsingName - error for file {} : {}", path, GetLastError());
			return false;
		}

		PIDLIST_ABSOLUTE pidlWatch;
		hr = SHGetIDListFromObject(psiItem, &pidlWatch);
		if (SUCCEEDED(hr))
		{
			PIDLIST_ABSOLUTE pidl = ILCreateFromPath(pszPath);
			SHChangeNotifyEntry const entries[] = { pidl, false/*fRecursive*/ };	// TODO: add recursive param

			int const nSources = SHCNRF_ShellLevel | SHCNRF_InterruptLevel | SHCNRF_NewDelivery;
			*handle = SHChangeNotifyRegister(hwnd, nSources, SHCNE_ALLEVENTS, STSHELL_USER_EVENT, ARRAYSIZE(entries), entries);
			hr = *handle != 0 ? S_OK : E_FAIL;

			CoTaskMemFree(pidlWatch);
		}
		else
		{
			ST_CORE_ERROR("SHChangeNotifyRegister - error {}", hr);
		}

		return hr == S_OK ? true : false;
	}

	bool ShellUtils::ChangeNotifyDegister(unsigned long handle)
	{
		if (handle)
		{
			SHChangeNotifyDeregister(handle);
			handle = 0;
		}

		return true;
	}

	void ShellUtils::ProcessEvent(WPARAM wParam, LPARAM lParam, ShellEvetData* shEvent)
	{
		HWND hwnd = GetWindowHandle();
		long event = lParam;
		PIDLIST_ABSOLUTE* pidl = (PIDLIST_ABSOLUTE*)wParam;
		HANDLE hNotifyLock = SHChangeNotification_Lock((HANDLE)wParam, (DWORD)lParam, &pidl, &event);
		if (hNotifyLock)
		{
			WCHAR szFileOld[MAX_PATH] = L"\0";
			WCHAR szFileNew[MAX_PATH] = L"\0";
			std::wstring wStrOld;
			std::wstring wStrNew;
			// check for event type
			if (event & (SHCNE_UPDATEITEM | SHCNE_CREATE | SHCNE_DELETE | SHCNE_RENAMEITEM))
			{
				IShellItem2* psi1 = NULL, * psi2 = NULL;

				if (pidl[0])
				{
					SHCreateItemFromIDList(pidl[0], IID_PPV_ARGS(&psi1));
					SHGetPathFromIDListW(pidl[0], szFileOld);
					wStrOld = szFileOld;
				}

				if (pidl[1])
				{
					SHCreateItemFromIDList(pidl[1], IID_PPV_ARGS(&psi2));
					SHGetPathFromIDListW(pidl[1], szFileNew);
					wStrNew = szFileNew;
				}

				std::string strOld(wStrOld.begin(), wStrOld.end());
				std::string strNew(wStrNew.begin(), wStrNew.end());

				shEvent->m_Event = event;
				shEvent->m_FilePath = strOld;
				shEvent->m_NewFilePath = strNew;
			}

			SHChangeNotification_Unlock(hNotifyLock);
		}
	}
}