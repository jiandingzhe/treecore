#
# configure file
#
configure_file(treejuce/Config.h.in treejuce/Config.h @ONLY)

#
# zlib
#
file(GLOB zlib_src_files
    "zlib/*.h"
    "zlib/*.c"
)

# add to a separate target, otherwise it won't use C compiler
add_library(treejuce_zlib STATIC ${zlib_src_files})

if(TREEJUCE_COMPILER STREQUAL "MSVC")
    # TODO
else()
    set_target_properties(treejuce_zlib
        PROPERTIES
        COMPILE_FLAGS -fPIC
    )
endif()

#
# treejuce
#
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

add_library(treejuce SHARED ${treejuce_common_src_files} ${treejuce_platform_src_files})

# link with dependent libs
if(TREEJUCE_OS STREQUAL "LINUX")
    target_link_libraries(treejuce pthread dl)
elseif(TREEJUCE_OS STREQUAL "WINDOWS")
    target_link_libraries(treejuce version winmm Shlwapi Dbghelp)
else()
	message(FATAL_ERROR "unimplemented TREEJUCE_OS: ${TREEJUCE_OS}")
endif()

target_link_libraries(treejuce
    treejuce_zlib
)

#
# install
#
if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS treejuce
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
    install(TARGETS treejuce
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
