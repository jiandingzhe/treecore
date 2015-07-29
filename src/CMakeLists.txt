#
# configure file
#
configure_file(treejuce/Config.h.in treejuce/Config.h @ONLY)

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
# treejuce
#

# collect source files
file(GLOB treejuce_common_src_files
    "treejuce/*.h"
    "treejuce/*.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    file(GLOB treejuce_platform_src_files
        "treejuce/native/linux_*.h"
        "treejuce/native/linux_*.cpp"
        "treejuce/native/posix_*.h"
        "treejuce/native/posix_*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(GLOB treejuce_platform_src_files
        "treejuce/native/win32_*.h"
        "treejuce/native/win32_*.cpp"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    file(GLOB treejuce_platform_src_files
        "treejuce/native/mac_*.mm"
        "treejuce/native/osx_*.h"
        "treejuce/native/osx_*.cpp"
        "treejuce/native/posix_*.h"
        "treejuce/native/posix_*.cpp"
    )
else()
    message(SEND_ERROR "unsupported target system: ${CMAKE_SYSTEM_NAME}")
endif()

# library target
add_library(treejuce SHARED 
    ${zlib_src_files}
    ${treejuce_common_src_files}
    ${treejuce_platform_src_files}
)

add_library(treejuce_s STATIC
    ${zlib_src_files}
    ${treejuce_common_src_files} 
    ${treejuce_platform_src_files}
)

# compile options
if(WIN32)
    set_target_properties(treejuce
        PROPERTIES
        COMPILE_DEFINITIONS JUCE_DLL_BUILD
    )
endif()

if(NOT MSVC)
    add_definitions(-fPIC)
endif()

# link with dependent libs
if(TREEJUCE_OS STREQUAL "LINUX")
    target_link_libraries(treejuce pthread dl)
elseif(TREEJUCE_OS STREQUAL "WINDOWS")
    target_link_libraries(treejuce version winmm Shlwapi Dbghelp)
else()
	message(FATAL_ERROR "unimplemented TREEJUCE_OS: ${TREEJUCE_OS}")
endif()

#
# utility binaries
#
add_executable(treejuce_bin_builder treejuce_bin_builder.cpp)
target_link_libraries(treejuce_bin_builder treejuce_s)
if(TREEJUCE_OS STREQUAL "LINUX")
    target_link_libraries(treejuce_bin_builder pthread dl)
elseif(TREEJUCE_OS STREQUAL "WINDOWS")
    target_link_libraries(treejuce_bin_builder version winmm Shlwapi Dbghelp)
endif()

#
# install
#
if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS treejuce treejuce_s treejuce_bin_builder
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES ${CMAKE_BINARY_DIR}/src/treejuce/Config.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treejuce
    )
    install(DIRECTORY treejuce/ treejuce/native/ treejuce/Atomic/ treejuce/SIMD/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/treejuce
        FILES_MATCHING PATTERN "*.h"
        PATTERN internal EXCLUDE
    )
else()
    install(TARGETS treejuce treejuce_s treejuce_bin_builder
        RUNTIME DESTINATION bin
		ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
    install(FILES ${CMAKE_BINARY_DIR}/src/treejuce/Config.h
        DESTINATION include/treejuce
    )
    install(DIRECTORY treejuce/ treejuce/native/ treejuce/Atomic/ treejuce/SIMD/
        DESTINATION include/treejuce
        FILES_MATCHING PATTERN "*.h"
        PATTERN internal EXCLUDE
    )
endif()
