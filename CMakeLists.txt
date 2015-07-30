cmake_minimum_required(VERSION 3.0)
project(treecore)

# if cmake >= 3.1, set policy for if
if(CMAKE_MAJOR_VERSION GREATER 2 AND CMAKE_MINOR_VERSION GREATER 0)
    cmake_policy(SET CMP0054 NEW)
endif()

set(TREECORE_VERSION_MAJOR 0)
set(TREECORE_VERSION_MINOR 0)
set(TREECORE_VERSION_PATCH 0)

#
# cpack
#
set(CPACK_PACKAGE_VERSION_MAJOR ${TREECORE_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${TREECORE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${TREECORE_VERSION_PATCH})

if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()

set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ".kdev4" "~" "build" "nbproject" "CMakeLists.txt.user*")

include(CPack)

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
else()
    message(FATAL_ERROR "unsupported target OS: ${CMAKE_SYSTEM_NAME}")
endif()

# unify compiler name
# GCC | MSVC | ICC | CLANG
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(TREECORE_CMAKE_COMPILER "GCC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(TREECORE_CMAKE_COMPILER "MSVC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    set(TREECORE_CMAKE_COMPILER "ICC")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(TREECORE_CMAKE_COMPILER "CLANG")
else()
    message(FATAL_ERROR "unsupported compiler family: ${CMAKE_CXX_COMPILER_ID}")
endif()

# get some type size
#include(CheckTypeSize)
#CHECK_TYPE_SIZE("void*" TREECORE_SIZE_PTR BUILTIN_TYPES_ONLY)
#if(NOT HAVE_TREECORE_SIZE_PTR)
#    message(FATAL_ERROR "do not have type void*,  WTF??!!")
#endif()
#if(NOT TREECORE_SIZE_PTR)
#    message(FATAL_ERROR "failed to get pointer size")
#endif()
#message(STATUS "pointer size: ${TREECORE_SIZE_PTR}")

# compiler specific flags
if(TREECORE_CMAKE_COMPILER STREQUAL "MSVC")
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(TREECORE_CMAKE_COMPILER STREQUAL "GCC" OR TREECORE_CMAKE_COMPILER STREQUAL "ICC" OR TREECORE_CMAKE_COMPILER STREQUAL "CLANG")
        add_definitions(-std=c++11)
        if(TREEFACE_CPU STREQUAL "X86_64")
                add_definitions(-msse2)
        else()
        endif()
endif()

# add build directory to inc
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/zlib
    ${CMAKE_BINARY_DIR}/src
)

add_subdirectory(src)
add_subdirectory(t)
