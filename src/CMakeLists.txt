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
    "treecore/impl/*.h"
    "treecore/Atomic/template.h"
    "treecore/SIMD/template.h"
)

# OS-specific source
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
    message(SEND_ERROR "unsupported target system: ${TREECORE_OS}")
endif()

# CPU-specific
if(TREECORE_CPU STREQUAL "X86")
    list(APPEND treecore_platform_src_files "treecore/SIMD/sse2.h")
else()
    message(SEND_ERROR "unsupported CPU: ${TREECORE_CPU}")
endif()

# compiler-specific
if(TREECORE_CMAKE_COMPILER STREQUAL "_GCC" OR TREECORE_CMAKE_COMPILER STREQUAL "_CLANG" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND (TREECORE_OS STREQUAL "LINUX" OR TREECORE_OS STREQUAL "OSX")))
    list(APPEND treecore_platform_src_files "treecore/Atomic/gcc.h")
    set_property(
        SOURCE
            ${treecore_common_src_files}
            ${treecore_platform_src_files}
        APPEND PROPERTY
            COMPILE_FLAGS -std=c++11
    )
elseif(TREECORE_CMAKE_COMPILER STREQUAL "_MSVC" OR (TREECORE_CMAKE_COMPILER STREQUAL "_ICC" AND TREECORE_OS STREQUAL "WINDOWS"))
    list(APPEND treecore_platform_src_files "treecore/Atomic/msvc.h")
else()
    message(SEND_ERROR "unsupported compiler: ${TREECORE_CMAKE_COMPILER}")
endif()

# declare library target
add_library(treecore STATIC
    ${zlib_src_files}
    ${treecore_common_src_files} 
    ${treecore_platform_src_files}
)
set_target_properties(treecore PROPERTIES
    DEBUG_POSTFIX          _dbg
    RELEASE_POSTFIX        _rel
    RELWITHDEBINFO_POSTFIX _reldbg
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
install(DIRECTORY treecore/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treecore
    FILES_MATCHING PATTERN "*.h"
    PATTERN internal EXCLUDE
)
