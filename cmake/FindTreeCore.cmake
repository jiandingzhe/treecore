include(FindPackageMessage)
include(FindPackageHandleStandardArgs)
include(GNUInstallDirs)

set(TREECORE_SEARCH_PREFIX $ENV{TREECORE_PREFIX} CACHE PATH "additional path to search for treecore")
set(TREECORE_SEARCH_STATIC 0 CACHE BOOL "search static library instead of shared library")

#
# find binary builder
#
find_program(TREECORE_BIN_BUILDER treecore_bin_builder
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_BINDIR}
        ${TREECORE_SEARCH_PREFIX}/bin
        $ENV{TREECORE_PREFIX}/bin
)

#
# find header directory
#

find_path(TREECORE_INCLUDE_DIR treecore/Config.h
    HINTS
        ${TREECORE_SEARCH_PREFIX_INC}
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
        ${TREECORE_SEARCH_PREFIX}/include
        $ENV{TREECORE_PREFIX}/include
)

#
# find library
#
if(NOT TREECORE_LIBRARIES)
    message("find treecore libraries")
    find_library(TREECORE_LIBRARY treecore
        HINTS
            ${TREECORE_SEARCH_PREFIX_LIB}
            ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
            ${TREECORE_SEARCH_PREFIX}/lib
            $ENV{TREECORE_PREFIX}/lib
    )
endif()

#
# finalize
#
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TREECORE
    REQUIRED_VARS
        TREECORE_BIN_BUILDER
        TREECORE_INCLUDE_DIR
        TREECORE_LIBRARY
)

#
# functions
#
function(treecore_wrap_resource input_dir class_name)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp
        COMMAND
            ${TREECORE_BIN_BUILDER} ${input_dir} ${CMAKE_CURRENT_BINARY_DIR} ${class_name}
    )
    add_custom_target(${class_name} ALL
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp
    )
endfunction()

function(target_use_treecore target_name)
    target_link_libraries(${target_name} ${TREECORE_LIBRARY})
    target_include_directories(${target_name} PUBLIC ${TREECORE_INCLUDE_DIR})
    treecore_set_dep_libraries(${target_name})
    treecore_set_compiler_options(${target_name})
    treecore_set_compiler_definitions(${target_name})
endfunction()
