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
    "treecore/simd/template.h"
)

file(GLOB treecore_internal_src_files
    treecore/internal/*.h
    treecore/internal/*.cpp
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
    list(APPEND treecore_platform_src_files "treecore/simd/sse2.h")
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
treecore_set_source_group(
    ${zlib_src_files}
    ${treecore_common_src_files}
    ${treecore_platform_src_files}
)
add_library(treecore STATIC
    ${zlib_src_files}
    ${treecore_common_src_files} 
    ${treecore_platform_src_files}
    ${PROJECT_SOURCE_DIR}/uncrustify.cfg
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

if(TREECORE_EMBEDDED)
    # as we are embedded, the resource wrapping function need to be specifically designed
    function(treecore_wrap_resource input_dir class_name)
        get_target_property(_bin_builder_path_ treecore_bin_builder OUTPUT_NAME)
        add_custom_command(
            OUTPUT
                ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h
                ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp
            COMMAND
                ${_bin_builder_path_} ${CMAKE_CURRENT_SOURCE_DIR}/${input_dir} ${CMAKE_CURRENT_BINARY_DIR} ${class_name}
        )
        add_custom_target(${class_name} ALL
            DEPENDS
                treecore_bin_builder
                ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h
                ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp
        )
    endfunction()
endif()

add_library(treecore_test_framework STATIC TreecoreTestFramework.cpp)
set_target_properties(treecore_test_framework PROPERTIES
    DEBUG_POSTFIX          _dbg
    RELEASE_POSTFIX        _rel
    RELWITHDEBINFO_POSTFIX _reldbg
)
target_use_treecore(treecore_test_framework)

#
# install
#
if(NOT TREECORE_EMBEDDED)
    install(TARGETS treecore treecore_bin_builder treecore_test_framework
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
endif()
