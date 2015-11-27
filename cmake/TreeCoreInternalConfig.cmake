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
