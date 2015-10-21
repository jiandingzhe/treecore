#ifndef TREECORE_CLASS_UTIL_H
#define TREECORE_CLASS_UTIL_H

/*****************************************************
        将此宏加入到类声明中，表明该类只能在栈上创建
******************************************************/
#define TREECORE_STACK_CLASS(classname) \
    void* operator new (std::size_t) = delete; \
    void* operator new (std::size_t, const std::nothrow_t&) = delete;\
    void* operator new[] (std::size_t)=delete; \
    void* operator new[] (std::size_t, const std::nothrow_t&)=delete; \
    void operator delete (void*) = delete; \
    void operator delete (void*, const std::nothrow_t&) = delete; \
    void operator delete[] (void*)=delete; \
    void operator delete[] (void*, const std::nothrow_t&)=delete;


/*******************************************************
        将此宏加入到类声明中，表明该类只提供static的方法，用户不能创建该类的实例
******************************************************/
#define TREECORE_FUNCTION_CLASS(classname) \
    classname()=delete; \
    ~classname()=delete; \
    TREECORE_STACK_CLASS(classname)


/*******************************************************
        将此宏加入到类声明中，该类将自动持有默认的构造函数，拷贝构造函数，赋值函数，析构函数
********************************************************/
#if JUCE_DEBUG
#define TREECORE_TRIVIAL_CLASS(classname) \
    classname(){}; \
    classname(const classname& other){ memcpy(this,&other,sizeof(classname)); };\
    classname& operator=(const classname& other){ memcpy(this,&other,sizeof(classname));return *this; };\
    ~classname(){} \
    JUCE_LEAK_DETECTOR(classname) //这个宏只管new和delete,所以拷贝构造和"="操作符不受影响
#else
#define TREECORE_TRIVIAL_CLASS(classname) \
    classname()=default; \
    classname(const classname& other)=default;\
    classname& operator=(const classname& other)=default;\
    ~classname()=default;
#endif

#define TREECORE_FORCE_TRIVIAL_CLASS(classname) \
    classname() = default; \
    classname( const classname& other ) = default; \
    classname& operator=( const classname& other ) = default; \
    ~classname() = default;

/*********************************************************
        将此宏加入到类声明中，该类将自动持有默认的构造函数，析构函数,但没有拷贝构造函数
******************************************************/
#define TREECORE_TRIVIAL_NOCOPYABLE_CLASS(classname) \
    classname() = default; \
    classname(const classname&) = delete; \
    classname& operator=( const classname& ) = delete; \
    ~classname() = default; \
    JUCE_LEAK_DETECTOR(classname)

/********************************************************
        将此宏加入到类声明中，该类将禁止拷贝构造函数和拷贝赋值函数
******************************************************/
#define TREECORE_NONCOPYABLE_CLASS(classname) \
    classname(const classname&)=delete;\
    classname& operator=(const classname&)=delete;\

#define TREECORE_FUNCTOR(returnType,functorName,...) \
    function<returnType(__VA_ARGS__)> functorName

#endif // TREECORE_CLASS_UTIL_H
