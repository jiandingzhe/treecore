set(TREECORE_INCLUDE_DIR
    ${treecore_SOURCE_DIR}/src 
    ${treecore_BINARY_DIR}/src 
    CACHE STRING ""
)

function(target_use_treecore target_name)
    target_link_libraries(${target_name} treecore)
    target_include_directories(${target_name} PRIVATE ${TREECORE_INCLUDE_DIR})
    treecore_set_dep_libraries(${target_name})
    treecore_set_compiler_options(${target_name})
    treecore_set_compiler_definitions(${target_name})
endfunction()

function(treecore_unit_test target_name)
    add_executable(${target_name} ${ARGN})
    target_link_libraries(${target_name} treecore_test_framework)
    target_use_treecore(${target_name})
    add_test(NAME ${target_name} COMMAND ${target_name})
endfunction()

function(treecore_wrap_resource input_dir class_name)
    file(GLOB_RECURSE _files_in_ ${input_dir}/*)
    set(_files_out_ ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp)

    # build-time wrap
    add_custom_command(
        DEPENDS ${_files_in_} treecore_bin_builder
        OUTPUT  ${_files_out_}
        COMMAND treecore_bin_builder --in ${CMAKE_CURRENT_SOURCE_DIR}/${input_dir} --out ${CMAKE_CURRENT_BINARY_DIR} --name ${class_name} --quiet
    )
    add_custom_target(${class_name} ALL
        DEPENDS ${_files_out_}
    )

    set_source_files_properties(${_files_out_} PROPERTIES GENERATED 1)
endfunction()

