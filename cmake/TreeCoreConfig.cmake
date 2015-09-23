# set default build type to Debug, in avoid of empty build type
if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE has no value, set to Debug")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Debug")
endif()

# remove MinSizeRel build type
if(DEFINED CMAKE_CONFIGURATION_TYPES)
    get_property(_config_types_ CACHE CMAKE_CONFIGURATION_TYPES PROPERTY VALUE)
    list(REMOVE_ITEM _config_types_ "MinSizeRel")
    set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY VALUE ${_config_types_})
endif()

# force /MT instead of /MD, in avoid of being fucked by MSVC
foreach(flag_var
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MT" tmp "${${flag_var}}")
        set(${flag_var} ${tmp} CACHE STRING "" FORCE)
    endif()
endforeach()

# unify processor name
# X86 | PPC | ARM
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(TREECORE_CPU "X86")
else()
    message(FATAL_ERROR "unsupported processor: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

# unify os name
# WINDOWS | LINUX | ANDROID | MAC | IOS
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TREECORE_OS "WINDOWS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TREECORE_OS "LINUX")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TREECORE_OS "OSX")
else()
    message(FATAL_ERROR "unsupported target OS: ${CMAKE_SYSTEM_NAME}")
endif()

# unify compiler name
# GCC | MSVC | ICC | CLANG
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TREECORE_CMAKE_COMPILER "_GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(TREECORE_CMAKE_COMPILER "_MSVC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(TREECORE_CMAKE_COMPILER "_ICC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(TREECORE_CMAKE_COMPILER "_CLANG")
else()
    message(FATAL_ERROR "unsupported compiler family: ${CMAKE_CXX_COMPILER_ID}")
endif()

function(treecore_set_dep_libraries target_name)
    if(TREECORE_OS STREQUAL "LINUX")
        target_link_libraries(${target_name} pthread dl)
    elseif(TREECORE_OS STREQUAL "WINDOWS")
        target_link_libraries(${target_name} version winmm Shlwapi Dbghelp)
    else()
            message(FATAL_ERROR "unimplemented TREECORE_OS: ${TREECORE_OS}")
    endif()
endfunction()

function(treecore_set_compiler_definitions target_name)
    if(MSVC)
            target_compile_definitions(${target_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    target_compile_definitions(${target_name} PRIVATE
        $<$<CONFIG:Debug>:          JUCE_LOG_ASSERTIONS=1 JUCE_CHECK_MEMORY_LEAKS=1>
        $<$<CONFIG:RelWithDebInfo>: JUCE_LOG_ASSERTIONS=1 JUCE_CHECK_MEMORY_LEAKS=1>
    )
endfunction()

function(treecore_set_compiler_options target_name)
    if(TREECORE_CMAKE_COMPILER STREQUAL "_MSVC" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND TREECORE_OS STREQUAL "WINDOWS"))
        target_compile_options(${target_name} PUBLIC "/wd4819")

    elseif(TREECORE_CMAKE_COMPILER STREQUAL "_GCC" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND (TREECORE_OS STREQUAL "LINUX" OR TREECORE_OS STREQUAL "OSX")) OR TREECORE_CMAKE_COMPILER STREQUAL "_CLANG")
        target_compile_options(${target_name} PUBLIC -std=c++11 -fPIC -msse3)

        if(TREEFACE_CPU STREQUAL "X86")
                target_compile_options(${target_name} PRIVATE -msse3 -mstackrealign)
        endif()
    endif()
endfunction()


