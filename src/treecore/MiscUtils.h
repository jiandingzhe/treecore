//
// utilities that don't need any preceding stuffs
//
#ifndef TREECORE_MISC_UTILS_H
#define TREECORE_MISC_UTILS_H

// debug flag
#ifndef NDEBUG
#    define TREECORE_DEBUG 1
#else
#    define TREECORE_DEBUG 0
#endif

// stringify utilities
#define _TREECORE_JOIN_MACRO_HELPER_( a, b ) a ## b
#define _TREECORE_STRINGIFY_MACRO_HELPER_( a ) #a

///
/// \brief stringify two items and join them into single string literal
///
/// If any of the stuffs are macros, they would be expanded firstly.
///
#define TREECORE_STRINGIFY_JOIN( item1, item2 )  _TREECORE_JOIN_MACRO_HELPER_( item1, item2 )

///
/// \brief make an item into string literal
///
/// It would be expanded if item is a macro.
///
#define TREECORE_STRINGIFY( item )  _TREECORE_STRINGIFY_MACRO_HELPER_( item )


#define TREECORE_VERSION_STRING TREECORE_STRINGIFY(TREECORE_VERSION_MAJOR) "." TREECORE_STRINGIFY(TREECORE_VERSION_MINOR) "." TREECORE_STRINGIFY(TREECORE_VERSION_PATCH)

#endif // TREECORE_MISC_UTILS_H
