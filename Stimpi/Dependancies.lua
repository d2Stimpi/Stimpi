VendorDir = {}
VendorDir["gtest"] = "%{wks.location}/vendor/googletest/googletest"

IncludeDir = {}
IncludeDir["mono"] = "%{wks.location}/vendor/mono/include"

LibraryDir = {}
LibraryDir["mono"] = "%{wks.location}/vendor/mono/lib/%{cfg.buildcfg}"


Library = {}
Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"
-- Mono dependancies
Library["Winmm"] = "Winmm.lib"
Library["Ws2"] = "Ws2_32.lib"
Library["Bcrypt"] = "Bcrypt.lib"
Library["Version"] = "Version.lib"

-- Windows dependancies
Library["Comctl32"] = "Comctl32.lib"
	