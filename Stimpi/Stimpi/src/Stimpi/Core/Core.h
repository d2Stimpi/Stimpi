#pragma once

//#define STB_IMAGE_IMPLEMENTATION

#ifdef STIMPI_BUILD_DLL
#define ST_API	__declspec (dllexport)
#else
#define ST_API	__declspec (dllimport)
#endif