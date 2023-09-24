#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include <chrono>
#include <thread>
#include <variant>
#include <map>

#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#ifdef STIMPI_BUILD_DLL
#define ST_API	__declspec (dllexport)
#else
#define ST_API	__declspec (dllimport)
#endif