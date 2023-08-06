#pragma once

#ifdef STIMPI_BUILD_DLL
#define STIMPI_API	__declspec (dllexport)
#else
#define STIMPI_API	__declspec (dllimport)
#endif