add_library(testframework STATIC TestFramework.cpp)

add_executable(t_hash_multi_map t_hash_multi_map.cpp)
target_link_libraries(t_hash_multi_map
    testframework
    treejuce
)

add_executable(t_hash_multi_map_move t_hash_multi_map_move.cpp)
target_link_libraries(t_hash_multi_map_move
    testframework
    treejuce
)

add_executable(t_hash_set t_hash_set.cpp)
target_link_libraries(t_hash_set
    testframework
    treejuce
)

add_executable(t_hash_set_string t_hash_set_string.cpp)
target_link_libraries(t_hash_set_string
    testframework
    treejuce
)

file(COPY zip_input.zip DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(t_zip_file t_zip_file.cpp)
target_link_libraries(t_zip_file
    testframework
    treejuce
)

add_executable(t_hash_map t_hash_map.cpp)
target_link_libraries(t_hash_map
    testframework
    treejuce
)

add_executable(t_var t_var.cpp)
target_link_libraries(t_var
    testframework
    treejuce
)

add_executable(t_string_hash t_string_hash.cpp)
target_link_libraries(t_string_hash
    testframework
    treejuce
)

if(WIN32)
    set_target_properties(t_var
        PROPERTIES
        COMPILE_DEFINITIONS JUCE_DLL
    )
endif()

add_executable(t_aligned_malloc t_aligned_malloc.cpp)
target_link_libraries(t_aligned_malloc
    testframework
    treejuce
)

add_executable(t_static_lib t_static_lib.cpp)
target_link_libraries(t_static_lib
    testframework
    treejuce_s
)
if(TREEJUCE_OS STREQUAL "LINUX")
    target_link_libraries(t_static_lib pthread dl)
elseif(TREEJUCE_OS STREQUAL "WINDOWS")
    target_link_libraries(t_static_lib treejuce version winmm Shlwapi Dbghelp)
endif()
