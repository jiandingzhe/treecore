include(FindPackageMessage)
include(FindPackageHandleStandardArgs)
include(TreeCoreConfig)
include(GNUInstallDirs)

set(TREECORE_SEARCH_PREFIX $ENV{TREECORE_PREFIX} CACHE PATH "additional path to search for treecore")

#
# find binary builder
#
find_program(TREECORE_BIN_BUILDER treecore_bin_builder
    HINTS
        ${TREECORE_SEARCH_PREFIX}/${CMAKE_INSTALL_BINDIR}
        ${TREECORE_SEARCH_PREFIX}/bin
)

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

#
# finalize
#
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TREECORE
    REQUIRED_VARS
        TREECORE_BIN_BUILDER
        TREECORE_INCLUDE_DIR

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
            ${TREECORE_BIN_BUILDER} ${CMAKE_CURRENT_SOURCE_DIR}/${input_dir} ${CMAKE_CURRENT_BINARY_DIR} ${class_name}
    )
    add_custom_target(${class_name} ALL
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.h
            ${CMAKE_CURRENT_BINARY_DIR}/${class_name}.cpp
    )
endfunction()

function(target_use_treecore target_name)
    target_link_libraries(${target_name} treecore)
    target_include_directories(${target_name} PRIVATE ${TREECORE_INCLUDE_DIR})
    treecore_set_dep_libraries(${target_name})
    treecore_set_compiler_options(${target_name})
    treecore_set_compiler_definitions(${target_name})
endfunction()
