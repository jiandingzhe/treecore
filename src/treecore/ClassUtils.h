#ifndef TREECORE_CLASS_UTIL_H
#define TREECORE_CLASS_UTIL_H

#include "treecore/PlatformDefs.h"

///
/// \brief disable class copy/move semantics
///
#define TREECORE_DECLARE_NON_COPYABLE( className ) \
    className( const className & ) = delete;       \
    className& operator = ( const className& ) = delete;

#define TREECORE_DECLARE_NON_MOVABLE( class_name ) \
    class_name( class_name && ) = delete;          \
    class_name& operator = ( class_name&& ) = delete;

/** This is a shorthand way of writing both a TREECORE_DECLARE_NON_COPYABLE and
    TREECORE_LEAK_DETECTOR macro for a class.
 */
#define TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( className ) \
    TREECORE_DECLARE_NON_COPYABLE( className )                        \
    TREECORE_LEAK_DETECTOR( className )
///
/// \brief prevent class to be used by heap allocation
///
/// This macro can be added to class definitions to disable the use of new/delete to
/// allocate the object on the heap, forcing it to only be used as a stack or member variable.
///
#define TREECORE_PREVENT_HEAP_ALLOCATION                                  \
    void* operator new ( std::size_t )   = delete;                        \
    void* operator new[] ( std::size_t ) = delete;                        \
    void* operator new ( std::size_t, const std::nothrow_t& )   = delete; \
    void* operator new[] ( std::size_t, const std::nothrow_t& ) = delete; \
    void operator delete ( void* )   = delete;                            \
    void operator delete[] ( void* ) = delete;                            \
    void operator delete ( void*, const std::nothrow_t& )   = delete;     \
    void operator delete[] ( void*, const std::nothrow_t& ) = delete;

///
/// \brief the class would only have static function members
///
#define TREECORE_FUNCTION_CLASS(_classname_) \
    _classname_()  = delete;      \
    ~_classname_() = delete;      \
    TREECORE_PREVENT_HEAP_ALLOCATION

/*******************************************************
        将此宏加入到类声明中，该类将自动持有默认的构造函数，拷贝构造函数，赋值函数，析构函数
 ********************************************************/
#define TREECORE_TRIVIAL_CLASS( classname )                     \
    classname() = default;                                      \
    classname( const classname &other ) = default;              \
    classname& operator = ( const classname& other ) = default; \
    ~classname() = default;

/*********************************************************
        将此宏加入到类声明中，该类将自动持有默认的构造函数，析构函数,但没有拷贝构造函数
 ******************************************************/
#define TREECORE_TRIVIAL_NOCOPYABLE_CLASS( _classname_ ) \
    _classname_()  = default;                            \
    ~_classname_() = default;                            \
    TREECORE_DECLARE_NON_COPYABLE( _classname_ )         \
    TREECORE_LEAK_DETECTOR( _classname_ )


#define TREECORE_FUNCTOR( returnType, functorName, ... ) \
    function<returnType( __VA_ARGS__ )> functorName

#endif // TREECORE_CLASS_UTIL_H
