add_library(testframework STATIC TestFramework.cpp)

add_executable(t_hash_multi_map t_hash_multi_map.cpp)
target_link_libraries(t_hash_multi_map
    testframework
    treejuce)

add_executable(t_hash_set t_hash_set.cpp)
target_link_libraries(t_hash_set
    testframework
    treejuce)
