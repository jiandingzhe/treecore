add_library(testframework STATIC TestFramework.cpp)

add_executable(t_hash_multi_map t_hash_multi_map.cpp)
target_link_libraries(t_hash_multi_map
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
