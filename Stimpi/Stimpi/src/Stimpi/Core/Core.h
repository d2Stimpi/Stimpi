#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <vector>

#ifdef STIMPI_BUILD_DLL
#define ST_API	__declspec (dllexport)
#else
#define ST_API	__declspec (dllimport)
#endif