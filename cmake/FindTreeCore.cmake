include(FindPackageMessage)
include(FindPackageHandleStandardArgs)
include(TreeCoreConfig)
include(GNUInstallDirs)

set(TREECORE_SEARCH_PREFIX $ENV{TREECORE_PREFIX} CACHE PATH "additional path to search for treecore")

#
# find header directory
#

find_path(TREECORE_INCLUDE_DIR treecore/Config.h
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}
        ${TREECORE_SEARCH_PREFIX}/include
)

#
# find library
#
message("find treecore libraries")
find_library(TREECORE_LIBRARY_DEBUG treecore_dbg
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

find_library(TREECORE_LIBRARY_RELWITHDEBINFO treecore_reldbg
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

find_library(TREECORE_LIBRARY_RELEASE treecore_rel
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

add_library(treecore STATIC IMPORTED GLOBAL)
set_target_properties(treecore
    PROPERTIES
        IMPORTED_LOCATION_DEBUG          ${TREECORE_LIBRARY_DEBUG}
        IMPORTED_LOCATION_GRAPHICDEBUG   ${TREECORE_LIBRARY_DEBUG}
        IMPORTED_LOCATION_RELEASE        ${TREECORE_LIBRARY_RELEASE}
        IMPORTED_LOCATION_RELWITHDEBINFO ${TREECORE_LIBRARY_RELWITHDEBINFO}
)

find_library(TREECORE_TEST_FRAMEWORK_DEBUG treecore_test_framework_dbg
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

find_library(TREECORE_TEST_FRAMEWORK_RELWITHDEBINFO treecore_test_framework_reldbg
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

find_library(TREECORE_TEST_FRAMEWORK_RELEASE treecore_test_framework_rel
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_LIBDIR}
        ${TREECORE_SEARCH_PREFIX}/lib
)

add_library(treecore_test_framework STATIC IMPORTED GLOBAL)
set_target_properties(treecore_test_framework
    PROPERTIES
        IMPORTED_LOCATION_DEBUG          ${TREECORE_TEST_FRAMEWORK_DEBUG}
        IMPORTED_LOCATION_GRAPHICDEBUG   ${TREECORE_TEST_FRAMEWORK_DEBUG}
        IMPORTED_LOCATION_RELEASE        ${TREECORE_TEST_FRAMEWORK_RELEASE}
        IMPORTED_LOCATION_RELWITHDEBINFO ${TREECORE_TEST_FRAMEWORK_RELWITHDEBINFO}
)

#
# find binary builder
#
find_program(TREECORE_BIN_BUILDER treecore_bin_builder
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_BINDIR}
        ${TREECORE_SEARCH_PREFIX}/bin
)

#
# finalize
#
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TREECORE
    REQUIRED_VARS
        TREECORE_BIN_BUILDER
        TREECORE_INCLUDE_DIR
        TREECORE_LIBRARY_DEBUG
        TREECORE_LIBRARY_RELEASE
        TREECORE_LIBRARY_RELWITHDEBINFO
        TREECORE_TEST_FRAMEWORK_DEBUG
        TREECORE_TEST_FRAMEWORK_RELEASE
        TREECORE_TEST_FRAMEWORK_RELWITHDEBINFO
)

#
# functions
#
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
    if(NOT TREECORE_BIN_BUILDER)
        message(FATAL_ERROR "cannot call treecore_wrap_resource because TREECORE_BIN_BUILDER is empty")
    endif()

    file(GLOB_RECURSE _files_in_ ${input_dir}/*)

    # collect output file by dry-run
    treecore_output_from_input(_files_out_ ${CMAKE_CURRENT_BINARY_DIR} ${_files_in_})
    list(APPEND _files_out_ ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h)

    # generation-time wrap
    execute_process(COMMAND ${TREECORE_BIN_BUILDER} --in ${CMAKE_CURRENT_SOURCE_DIR}/${input_dir} --out ${CMAKE_CURRENT_BINARY_DIR} --name ${class_name} --update --quiet)

    # build-time wrap
    add_custom_command(
        DEPENDS ${_files_in_}
        OUTPUT  ${_files_out_}
        COMMAND ${TREECORE_BIN_BUILDER} --in ${CMAKE_CURRENT_SOURCE_DIR}/${input_dir} --out ${CMAKE_CURRENT_BINARY_DIR} --name ${class_name} --quiet
    )
    add_custom_target(${class_name} ALL
        DEPENDS ${_files_out_}
    )

    set_source_files_properties(${_files_out_} PROPERTIES GENERATED 1)
endfunction()
