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

# add to a separate target
# otherwise it won't use C compiler
add_library(treeface_zlib STATIC ${zlib_src_files})

if(TREEJUCE_COMPILER STREQUAL "MSVC")
        # TODO
else()
        set_target_properties(treeface_zlib
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

add_library(treejuce ${treejuce_common_src_files} ${treejuce_platform_src_files})

if(TREEFACE_OS STREQUAL "LINUX")
    target_link_libraries(treejuce pthread dl)
elseif(TREEFACE_OS STREQUAL "WINDOWS")
    target_link_libraries(treejuce version winmm Shlwapi Dbghelp)
endif()

target_link_libraries(treejuce
    treeface_zlib
)

