#ifndef ztd_static_array_h__tyrdyt
#define ztd_static_array_h__tyrdyt

#include <cstddef>
#include <initializer_list>
#include <type_traits>

#include "treecore/MPL.h"
#include "treecore/StandardHeader.h"
#include "treecore/Align.h"

namespace treecore
{


namespace helper
{

template<typename SizeQueue  >
struct SizeCounter {
    enum {
        SIZE = SizeQueue::head * SizeCounter<typename SizeQueue::pop_head>::SIZE,
        ThisSize = SizeQueue::head,
        Dim = SizeCounter<typename SizeQueue::pop_head>::Dim+1
    };
};

template<>
struct SizeCounter < void > {
    enum {
        SIZE = 1,
        ThisSize = 0,
        Dim = -1
    };
};

} // namespace helper


/*
        之所以不想用 data=1; 这种初始化方式,而是采用 data={1}的方式,是因为当数组超过1维时,可能出现这种情况:
        static_array<int,2,3,4> data;
        data[0][0]=10;.....这里看起来放佛是某一个元素的赋值,实际上却是整个第零维全体赋值,相反的,写成这样是没问题的:
        data[0][0]={10}; ...... 至少看起来不像是给某个特定元素赋值.

        请注意,一维数组没有这个问题,因为对一维数组来说,data[0]肯定指的是某一个元素,所以在一维的情况下我们把
        data=1这种赋值方式启动了

*/

template<typename T, typename SizeQueue>
class static_array_pod_imp
{
public:
    typedef helper::SizeCounter<SizeQueue> SizeCountThisTime;
    enum : size_t { InitSize0 = SizeCountThisTime::ThisSize };
    enum : size_t { size = SizeCountThisTime::SIZE };
    enum : size_t { Dim = SizeCountThisTime::Dim };
    typedef typename mpl_type_if<SizeQueue::number == 1, T, static_array_pod_imp<T, typename SizeQueue::pop_head> >::type ChildType;

public:
    static_array_pod_imp() = default;
    static_array_pod_imp(const static_array_pod_imp&) =  default;
    static_array_pod_imp& operator=( const static_array_pod_imp& ) = default;
    ~static_array_pod_imp() = default;

    template<typename InitType>
    static_array_pod_imp(InitType value, typename std::enable_if< std::is_assignable<T&,InitType>::value && Dim == 0 >::type * = nullptr) noexcept
    {
        setAll(value);
    }

    template<typename InitType>
    static_array_pod_imp(const std::initializer_list<InitType>& init, typename std::enable_if<std::is_assignable<T&, InitType>::value>::type * = nullptr) noexcept
    {
        init.size()==1 ? setAll(*init.begin()) : setAllFromInitList(init);
    }

    template<typename InitType, typename EnableType = typename std::enable_if<std::is_assignable<T&,InitType>::value && Dim == 0>::type>
    static_array_pod_imp& operator=(InitType value) noexcept
    {
        setAll(value);
        return *this;
    }

    template<typename InitType,typename EnableType = typename std::enable_if<std::is_assignable<T&,InitType>::value>::type>
    static_array_pod_imp& operator=(const std::initializer_list<InitType>& init) noexcept
    {
        init.size()==1 ? setAll(*init.begin()) : setAllFromInitList(init);
        return *this;
    }

    template<typename IntType>
    ChildType& operator[](IntType i) noexcept
    {
        check();
        //static_assert(std::is_integral<IntType>::value,"1111111");
        jassert(i < (IntType)InitSize0);
        jassert( (int)i >= 0 );
        return m_data[i];
    }

    template<typename IntType>
    const ChildType& operator[](IntType i) const noexcept
    {
        check();
        //static_assert(std::is_integral<IntType>::value,"1111111");
        jassert(i < (IntType)InitSize0);
        jassert( (int)i >= 0 );
        return m_data[i];
    }

    operator T* () noexcept
    {
        return get();
    }

    forcedinline operator const T* () const noexcept
    {
        return get();
    }

    forcedinline void clear() noexcept
    {
        check();
        zeromem(get(),size*sizeof(T));
    }

    forcedinline T* get() { check(); return (T*)m_data; }

    forcedinline const T* get() const { check(); return (const T*)m_data; }

    forcedinline int indexOfAddress(T* p) const
    {
        const T*const head = m_data;
        return p - head;
    }

private:
    ChildType m_data[InitSize0];

    //typename aligned_type<ChildType,ALIGNOF(T),InitSize0>::array m_data;

    forcedinline void setAll(const T& init) noexcept
    {
        check();
        for(size_t i = 0; i < size; ++i) {
            get()[i] = init;
        }
    }
    template<typename InitType>
    forcedinline void setAllFromInitList(const std::initializer_list<InitType>& init) noexcept
    {
        check();
        jassert(init.size() == size);
        T* k = get();
        for(const auto& e : init) {
            *( k++ ) = e;
        }
    }

    static void check()
    {
        static_assert( size > 0,"size have to >0,the reason.... I think you know" );
        static_assert( sizeof( static_array_pod_imp<T,SizeQueue> ) == sizeof(T)*size,"normally all cpp complier will be OK here" );
    }

};


//=================================================================================================================================================================================================================================================================================================================================================

template<typename T,typename SizeQueue>
class static_array_imp
{
public:
    typedef helper::SizeCounter<SizeQueue> SizeCountThisTime;
    enum: size_t { InitSize0 = SizeCountThisTime::ThisSize };
    enum: size_t { size = SizeCountThisTime::SIZE };
    enum: size_t { Dim = SizeCountThisTime::Dim };
    typedef typename mpl_type_if<
    SizeQueue::number == 1,
    T,
    static_array_imp<T,typename SizeQueue::pop_head>
    >::type ChildType;
public:
    template<typename... InitArgs>
    static_array_imp(InitArgs... args)
    {
        for( size_t i = 0; i<size; ++i ) new( m_rawArray + i ) T( args... );
    };

    static_array_imp(const static_array_imp& other)
    {
        for( size_t i = 0; i<size; ++i ) new( m_rawArray + i ) T( other.get()[i] );
    }

    static_array_imp(static_array_imp&& other)
    {
        for( size_t i = 0; i < size; ++i ) new( m_rawArray + i ) T( move( other.get()[i] ) );
    }

    template<typename InitType>
    static_array_imp(InitType init,typename std::enable_if<std::is_assignable<T&,InitType>::value&&Dim==0>::type * = nullptr)
    {
        for( size_t i = 0; i<size; ++i ) new( m_rawArray + i ) T( init );
    };

    template<typename InitType>
    static_array_imp(const std::initializer_list<InitType>& initList, typename std::enable_if<std::is_assignable<T&, InitType>::value>::type * = nullptr) noexcept
    {
        initList.size()==1 ? CreateAll(initList) : CreateAllFromInitList(initList);
    };

    static_array_imp& operator=( const static_array_imp& other )
    {
        for( size_t i = 0; i < size; ++i ) get()[i] = other.get()[i];
        return *this;
    }

    static_array_imp& operator=( static_array_imp&& other )
    {
        for( size_t i = 0; i < size; ++i ) get()[i] = move( other.get()[i] );
        return *this;
    }

    ~static_array_imp()
    {
        for(size_t i = 0; i < size; ++i) get()[i].~T();
    };

    template<typename InitType,typename EnableType = typename std::enable_if<std::is_assignable<T&,InitType>::value&&Dim==0>::type>
    static_array_imp& operator=( InitType value )
    {
        for(int i=0;i<size;++i) get()[i]=value;
        return *this;
    }

    template<typename InitType,typename EnableType = typename std::enable_if<std::is_assignable<T&,InitType>::value>::type>
    static_array_imp& operator=( const std::initializer_list<InitType>& initList ) noexcept
    {
        initList.size()==1 ? SetAll(initList) : SetAllFromInitList(initList);
        return *this;
    }

    template<typename IntType>
    forcedinline ChildType& operator[](IntType i) noexcept
    {
        return (ChildType&)m_rawArray[i];
    }

    template<typename IntType>
    forcedinline const ChildType& operator[](IntType i) const noexcept
    {
        return (ChildType&)m_rawArray[i];
    }

    forcedinline operator T* () noexcept
    {
        return (T*)m_rawArray.get();
    }

    forcedinline operator const T* () const noexcept
    {
        return (T*)m_rawArray.get();
    }

    forcedinline T* get() { return (T*)m_rawArray.get(); }

    forcedinline const T* get() const { return (const T*)m_rawArray.get(); }

private:

    typedef typename std::aligned_storage<sizeof(T), TREECORE_ALIGNOF(T)>::type Temp;

    static_array_pod_imp<Temp,SizeQueue> m_rawArray;
//    typename aligned_type< static_array_pod_imp<Temp,SizeQueue>, ALIGNOF(T) >::type m_rawArray;

    template<typename InitType>
    void SetAll(const std::initializer_list<InitType>& initList)
    {
        jassert(initList.size() == 1);
        T* k = get();
        const InitType& e=*initList.begin();
        for(size_t i=0;i<size;++i) {
            *( k++ ) = e;
        }
    }

    template<typename InitType>
    void SetAllFromInitList( const std::initializer_list<InitType>& initList )
    {
        jassert(initList.size() == size);
        T* k = get();
        for(const auto& e : initList) {
            *( k++ ) = e;
        }
    }

    template<typename InitType>
    void CreateAll(const std::initializer_list<InitType>& initList)
    {
        jassert(initList.size() == 1);
        const InitType& e = *initList.begin();
        for(int i = 0; i < size; ++i) {
            // 如果你的编译在此处失败,这可能是由于你的T类型并不支持拷贝构造和右值构造,而你
            // 正在试图用初始化列表来初始化或者拷贝,要记住,初始化列表里面的东西全都是以const T&的
            // 形式存在的,所以如果使用初始化列表,T类必须支持const T&拷贝(或者右值拷贝?)
            new(m_rawArray+i) T(e);
        }
    }

    template<typename InitType>
    void CreateAllFromInitList(const std::initializer_list<InitType>& initList)
    {
        jassert(initList.size() == size);
        int i=0;
        for(const auto& e : initList) {
            // 如果你的编译在此处失败,这可能是由于你的T类型并不支持拷贝构造和右值构造,而你
            // 正在试图用初始化列表来初始化或者拷贝,要记住,初始化列表里面的东西全都是以const T&的
            // 形式存在的,所以如果使用初始化列表,T类必须支持const T&拷贝(或者右值拷贝?)
            new(m_rawArray+i) T(e);
            ++i;
        }
    }

};


//=================================================================================================================================================================================================================================================================================================================================================



template<typename T, size_t... Sizes>
using StaticArray = typename mpl_type_if<
!std::is_trivial<T>::value,
static_array_imp<T,mpl_value_queue<Sizes...>>,
static_array_pod_imp<T,mpl_value_queue<Sizes...>>
>::type;




} // namespace treecore

#endif // ztd_static_array_h__
