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

if(TREECORE_OS STREQUAL "LINUX")
    file(GLOB treecore_platform_src_files
        "treecore/native/linux_*.h"
        "treecore/native/linux_*.cpp"
        "treecore/native/posix_*.h"
        "treecore/native/posix_*.cpp"
    )
elseif(TREECORE_OS STREQUAL "WINDOWS")
    file(GLOB treecore_platform_src_files
        "treecore/native/win32_*.h"
        "treecore/native/win32_*.cpp"
    )
elseif(TREECORE_OS STREQUAL "OSX")
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
add_library(treecore STATIC
    ${zlib_src_files}
    ${treecore_common_src_files} 
    ${treecore_platform_src_files}
)

treecore_set_compiler_definitions(treecore)
treecore_set_compiler_options(treecore)
target_include_directories(treecore PUBLIC
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/zlib
    ${PROJECT_BINARY_DIR}/src
)

#
# utility binaries
#
add_executable(treecore_bin_builder treecore_bin_builder.cpp)
target_use_treecore(treecore_bin_builder)

#
# install
#
include(GNUInstallDirs)
install(TARGETS treecore treecore_bin_builder
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
