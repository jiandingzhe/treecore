# determine whether treecore is embedded
# by check if treecore's project dir is toplevel project dir
set(TREECORE_EMBEDDED 0 CACHE INTERNAL "Whether treecore is toplevel project or an embedded subproject.")
if (NOT (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    set_property(CACHE TREECORE_EMBEDDED PROPERTY VALUE 1)
endif()

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
    set(TREECORE_OS "WINDOWS" CACHE STRING "" FORCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TREECORE_OS "LINUX" CACHE STRING "" FORCE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TREECORE_OS "OSX" CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "unsupported target OS: ${CMAKE_SYSTEM_NAME}")
endif()

# unify compiler name
# GCC | MSVC | ICC | CLANG
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TREECORE_CMAKE_COMPILER "_GCC" CACHE STRING "" FORCE)
elseif(MSVC)
    set(TREECORE_CMAKE_COMPILER "_MSVC" CACHE STRING "" FORCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(TREECORE_CMAKE_COMPILER "_ICC" CACHE STRING "" FORCE)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(TREECORE_CMAKE_COMPILER "_CLANG" CACHE STRING "" FORCE)
else()
    message(FATAL_ERROR "unsupported compiler family: ${CMAKE_CXX_COMPILER_ID}")
endif()

function(treecore_set_dep_libraries target_name)
    if(TREECORE_OS STREQUAL "LINUX")
        target_link_libraries(${target_name} pthread dl)
    elseif(TREECORE_OS STREQUAL "WINDOWS")
        target_link_libraries(${target_name} version winmm Shlwapi Dbghelp)
    elseif(TREECORE_OS STREQUAL "OSX")
        find_library(FRAMELIB_ACCELERATE Accelerate)
        find_library(FRAMELIB_AUDIOTOOLBOX AudioToolbox)
        find_library(FRAMELIB_AUDIOUNIT AudioUnit)
        find_library(FRAMELIB_CARBON Carbon)
        find_library(FRAMELIB_COCOA Cocoa)
        find_library(FRAMELIB_COREAUDIO CoreAudio)
        find_library(FRAMELIB_COREAUDIOKIT CoreAudioKit)
        find_library(FRAMELIB_COREMIDI CoreMIDI)
        find_library(FRAMELIB_DISCRECORDING DiscRecording)
        find_library(FRAMELIB_IOKIT IOKit)
        find_library(FRAMELIB_QUARTZCORE QuartzCore)
        find_library(FRAMELIB_WEBKIT WebKit)

        target_link_libraries(${target_name}
            ${OPENGL_gl_LIBRARY}
            ${FRAMELIB_ACCELERATE}
            ${FRAMELIB_AUDIOTOOLBOX}
            ${FRAMELIB_AUDIOUNIT}
            ${FRAMELIB_CARBON}
            ${FRAMELIB_COCOA}
            ${FRAMELIB_COREAUDIO}
            ${FRAMELIB_COREAUDIOKIT}
            ${FRAMELIB_COREMIDI}
            ${FRAMELIB_DISCRECORDING}
            ${FRAMELIB_IOKIT}
            ${FRAMELIB_QUARTZCORE}
            ${FRAMELIB_WEBKIT}
        )
    endif()
endfunction()

function(treecore_set_compiler_definitions target_name)
    if(WIN32)
        target_compile_definitions(${target_name} PRIVATE
            _CRT_SECURE_NO_WARNINGS
            STRICT=1
            _UNICODE=1
            UNICODE=1
            WIN32_LEAN_AND_MEAN=1)
    endif()

    target_compile_definitions(${target_name} PRIVATE
        $<$<CONFIG:Debug>:          TREECORE_LOG_ASSERTIONS=1 TREECORE_CHECK_MEMORY_LEAKS=1>
        $<$<CONFIG:RelWithDebInfo>: TREECORE_LOG_ASSERTIONS=1 TREECORE_CHECK_MEMORY_LEAKS=0>
        $<$<CONFIG:Release>:        TREECORE_LOG_ASSERTIONS=0 TREECORE_CHECK_MEMORY_LEAKS=0>
    )
endfunction()

function(treecore_set_compiler_options target_name)
    if(TREECORE_CMAKE_COMPILER STREQUAL "_MSVC" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND TREECORE_OS STREQUAL "WINDOWS"))
        target_compile_options(${target_name} PUBLIC "/wd4819" "/wd4800")

    elseif(TREECORE_CMAKE_COMPILER STREQUAL "_GCC" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND (TREECORE_OS STREQUAL "LINUX" OR TREECORE_OS STREQUAL "OSX")) OR TREECORE_CMAKE_COMPILER STREQUAL "_CLANG")
        target_compile_options(${target_name} PUBLIC -fPIC)
        target_compile_options(${target_name} INTERFACE -std=c++11)

        if(TREECORE_CPU STREQUAL "X86")
            target_compile_options(${target_name} PRIVATE -msse3 -mstackrealign)
        endif()
    endif()

    if(TREECORE_OS STREQUAL "OSX" AND TREECORE_CMAKE_COMPILER STREQUAL "_CLANG")
        target_compile_options(${target_name} PUBLIC -fmodules)
    endif()
endfunction()

function(treecore_output_from_input var_out dir_out)
    foreach(f_in ${ARGN})
        get_filename_component(_base_name_ ${f_in} NAME)
        list(APPEND result "${dir_out}/${_base_name_}.cpp")
    endforeach()
    set(${var_out} ${result} PARENT_SCOPE)
endfunction()

function(treecore_set_source_group)
    foreach(curr_file ${ARGN})
        if(IS_ABSOLUTE ${curr_file})
            file(RELATIVE_PATH curr_file_rel ${CMAKE_CURRENT_SOURCE_DIR} ${curr_file})
            if(curr_file_rel)
                get_filename_component(curr_file_dir ${curr_file_rel} DIRECTORY)
                string(REPLACE "/" "\\" curr_file_group ${curr_file_dir})
                source_group(${curr_file_group} FILES ${curr_file})
            endif()
        else()
            get_filename_component(curr_file_dir ${curr_file} DIRECTORY)
            string(REPLACE "/" "\\" curr_file_group ${curr_file_dir})
            source_group(${curr_file_group} FILES ${curr_file})
        endif()
    endforeach()
endfunction()
