add_library(testframework STATIC TestFramework.cpp)
if(NOT MSVC)
    target_compile_options(testframework PRIVATE -std=c++11)
endif()

enable_testing()

foreach(test_name
    t_hash_multi_map
    t_hash_multi_map_move
    t_hash_set
    t_hash_set_string
    t_zip_file
    t_hash_map
    t_var
    t_aligned_malloc
    t_aligned_array
    t_sorted_set
    t_queue
)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} testframework)
    target_use_treecore(${test_name})
    add_test(${test_name} ${test_name})
endforeach()
