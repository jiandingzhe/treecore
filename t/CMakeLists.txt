configure_file(TestConfig.h.in TestConfig.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

treecore_wrap_resource(build_time_resource_wrap_input build_time_resource_wrap_output)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/zip_input.zip DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

foreach(test_name
    t_abstract_fifo
    t_aligned_array
    t_aligned_malloc
    t_array_elem_move
    t_array_ref
    t_atomic_func_st
    t_atomic_obj_st
    t_build_time_resource_wrap
    t_child_process
    t_dlist
    t_file
    t_float_utils
    t_fxsave
    t_gzip_compressor_output_stream
    t_hash_multi_map
    t_hash_multi_map_move
    t_hash_set
    t_hash_set_string
    t_hash_map
    t_hash_map_value_move
    t_identifier
    t_int_utils
    t_int_type
    t_json
    t_lf_queue_st
    t_memory_input_output_stream
    t_mpl
    t_obj_pool
    t_opt_scope_ptr
    t_option_parser
    t_queue
    t_ref_count_holder_st
    t_ref_count_singleton_mt
    t_ref_count_singleton_st
    t_scoped_denormal_flag
    t_simd_64
    t_simd_128
    t_simd_obj_128
    t_sorted_set
    t_sparse_set
    t_static_array
    t_string
    t_string_pool
    t_text_diff
    t_thread
    t_var
    t_weak_ptr
    t_zip_file
)
    treecore_unit_test(${test_name} ${test_name}.cpp)
endforeach()

add_executable(t_binary_builder t_binary_builder.cpp)
target_link_libraries(t_binary_builder treecore_test_framework)
target_use_treecore(t_binary_builder)
add_test(NAME t_binary_builder COMMAND t_binary_builder $<TARGET_FILE:treecore_bin_builder>)
