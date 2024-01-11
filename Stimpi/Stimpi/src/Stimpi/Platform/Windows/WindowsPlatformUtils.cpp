#include "stpch.h"
#include "Stimpi/Utils/PlatformUtils.h"

#include "Stimpi/Log.h"

#include "Windows.h"
#include <ShlObj_core.h>
#include <ShObjIdl.h>
#include <commdlg.h>
#include <SDL.h>
#include <SDL_syswm.h>

namespace Stimpi
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		SDL_Window* sdlWindow = SDL_GL_GetCurrentWindow();
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;

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
		SDL_Window* sdlWindow = SDL_GL_GetCurrentWindow();
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;

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
		SDL_Window* sdlWindow = SDL_GL_GetCurrentWindow();
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;

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
}