add_library(testframework STATIC TestFramework.cpp)
if(NOT MSVC)
    target_compile_options(testframework PUBLIC -std=c++11)
endif()

enable_testing()

foreach(test_name
    t_aligned_array
    t_aligned_malloc
    t_atomic_func_st
    t_atomic_obj_st
    t_hash_multi_map
    t_hash_multi_map_move
    t_hash_set
    t_hash_set_string
    t_hash_map
    t_identifier
    t_lf_queue_st
    t_obj_pool
    t_queue
    t_ref_count_singleton_st
    t_sorted_set
    t_string_pool
    t_var
    t_zip_file
)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} testframework)
    target_use_treecore(${test_name})
    add_test(${test_name} ${test_name})
endforeach()
