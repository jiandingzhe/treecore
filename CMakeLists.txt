cmake_minimum_required(VERSION 3.0)
project(treecore)


# include CMake modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(TreeCoreConfig)
include(TreeCoreInternalConfig)
include(GNUInstallDirs)

# add build directory to inc
add_subdirectory(src)
add_subdirectory(t)
