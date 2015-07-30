#
# configure file
#
configure_file(treecore/Config.h.in treecore/Config.h @ONLY)

#
# zlib
#
file(GLOB zlib_src_files
    "zlib/*.c"
)

set_source_files_properties(${zlib_src_files}
    PROPERTIES
        LANGUAGE C
)

#
# treecore
#

# collect source files
file(GLOB treecore_common_src_files
    "treecore/*.h"
    "treecore/*.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB treecore_platform_src_files
        "treecore/native/linux_*.h"
        "treecore/native/linux_*.cpp"
        "treecore/native/posix_*.h"
        "treecore/native/posix_*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB treecore_platform_src_files
        "treecore/native/win32_*.h"
        "treecore/native/win32_*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB treecore_platform_src_files
        "treecore/native/mac_*.mm"
        "treecore/native/osx_*.h"
        "treecore/native/osx_*.cpp"
        "treecore/native/posix_*.h"
        "treecore/native/posix_*.cpp"
    )
else()
    message(SEND_ERROR "unsupported target system: ${CMAKE_SYSTEM_NAME}")
endif()

# library target
add_library(treecore SHARED 
    ${zlib_src_files}
    ${treecore_common_src_files}
    ${treecore_platform_src_files}
)

add_library(treecore_s STATIC
    ${zlib_src_files}
    ${treecore_common_src_files} 
    ${treecore_platform_src_files}
)

# compile options
if(WIN32)
    set_target_properties(treecore
        PROPERTIES
        COMPILE_DEFINITIONS JUCE_DLL_BUILD
    )
endif()

if(NOT MSVC)
    add_definitions(-fPIC)
endif()

# link with dependent libs
if(TREECORE_OS STREQUAL "LINUX")
    target_link_libraries(treecore pthread dl)
elseif(TREECORE_OS STREQUAL "WINDOWS")
    target_link_libraries(treecore version winmm Shlwapi Dbghelp)
else()
	message(FATAL_ERROR "unimplemented TREECORE_OS: ${TREECORE_OS}")
endif()

#
# utility binaries
#
add_executable(treecore_bin_builder treecore_bin_builder.cpp)
target_link_libraries(treecore_bin_builder treecore_s)
if(TREECORE_OS STREQUAL "LINUX")
    target_link_libraries(treecore_bin_builder pthread dl)
elseif(TREECORE_OS STREQUAL "WINDOWS")
    target_link_libraries(treecore_bin_builder version winmm Shlwapi Dbghelp)
endif()

#
# install
#
if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS treecore treecore_s treecore_bin_builder
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES ${CMAKE_BINARY_DIR}/src/treecore/Config.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treecore
    )
    install(DIRECTORY treecore/ treecore/native/ treecore/Atomic/ treecore/SIMD/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treecore
        FILES_MATCHING PATTERN "*.h"
        PATTERN internal EXCLUDE
    )
else()
    install(TARGETS treecore treecore_s treecore_bin_builder
        RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
    install(FILES ${CMAKE_BINARY_DIR}/src/treecore/Config.h
        DESTINATION include/treecore
    )
    install(DIRECTORY treecore/ treecore/native/ treecore/Atomic/ treecore/SIMD/
        DESTINATION include/treecore
        FILES_MATCHING PATTERN "*.h"
        PATTERN internal EXCLUDE
    )
endif()
