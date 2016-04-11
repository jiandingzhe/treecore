cmake_minimum_required(VERSION 3.0)
project(treecore)
enable_testing()

set(TREECORE_VERSION_MAJOR 0)
set(TREECORE_VERSION_MINOR 1)
set(TREECORE_VERSION_PATCH 0)

# include CMake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(TreeCoreConfig)
include(TreeCoreInternalConfig)
include(GNUInstallDirs)

# add build directory to inc
add_subdirectory(src)

if(NOT TREECORE_EMBEDDED)
    add_subdirectory(t)
    add_subdirectory(test)
endif()

# install cmake modules
if(NOT TREECORE_EMBEDDED)
    install(
        FILES
            cmake/FindTreeCore.cmake
            cmake/TreeCoreConfig.cmake
        DESTINATION
            ${CMAKE_ROOT}/Modules)
endif()
