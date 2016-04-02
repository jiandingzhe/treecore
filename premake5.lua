-- premake5.lua
workspace "treecore"
    language "C++"
    configurations { "Debug32", "Debug64", "RelWithDebugInfo32", "RelWithDebugInfo64", "Release32" , "Release64" }

    filter "configurations:*32"
        architecture "x86"

    filter "configurations:*64"
        architecture "x86_64"

project "treecore"
    kind "StaticLib"
    includedirs { "src/" }
    files { "src/Headers/*.h", "src/Headers/*.cpp" }
    files { "src/Headers/Unils/*.h", "src/Headers/Unils/*.cpp" }
    files { "src/MPL/*.h", "src/MPL/*.cpp" }
    files { "src/Atomic/*.h", "src/Atomic/*.cpp" }
    files { "src/Memory/*.h", "src/Memory/*.cpp" }
    files { "src/App/*.h", "src/App/*.cpp" }
    
filter "system:Windows"
    files { "src/Headers/WindowsHeaders/*.h", "src/Headers/WindowsHeaders/*.cpp" }

filter "system:MacOSX"
    files { "src/Headers/MacHeaders/*.h" , "src/Headers/MacHeaders/*.cpp" }
    
project "hello_world"
    kind "ConsoleApp"

--[[
project "Test"
kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
files { "**.h", "**.c" }

project "HelloWorld"
   kind "ConsoleApp"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   files { "**.h", "**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      flags { "Symbols" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
      
      --]]