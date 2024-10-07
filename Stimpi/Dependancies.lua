VendorDir = {}
VendorDir["gtest"] = "%{wks.location}/vendor/googletest/googletest"

IncludeDir = {}
IncludeDir["mono"] = "%{wks.location}/vendor/mono/include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/vendor/mono/lib/%{cfg.buildcfg}"
LibraryDir["sdl2"] = "%{wks.location}/vendor/SDL2-2.0.20/lib/x64"
LibraryDir["yaml"] = "%{wks.location}/vendor/yaml-cpp/Debug"

Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
-- Mono dependancies
Library["Winmm"] = "Winmm.lib"
Library["Ws2"] = "Ws2_32.lib"
Library["Bcrypt"] = "Bcrypt.lib"
Library["Version"] = "Version.lib"
Library["sdl2"] = "%{LibraryDir.sdl2}/SDL2.dll"
Library["yaml"] = "%{LibraryDir.yaml}/yaml-cppd.dll"

-- Windows dependancies
Library["Comctl32"] = "Comctl32.lib"
	