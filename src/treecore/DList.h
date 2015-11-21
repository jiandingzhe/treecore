#ifndef ztd_dlistnode_h__
#define ztd_dlistnode_h__

#include "treecore/SimdFunc.h"
#include "treecore/StaticArray.h"
#include "treecore/StandardHeader.h"
#include "treecore/LeakedObjectDetector.h"

namespace treecore
{

//=================================================================================================================================================================================================================================================================================================================================================

/*
        我们有多种方式去操作一条双向链表:
        1.通过节点操作
        2.通过链表类本身操作
        3.通过迭代器操作

        但是,这样有个问题,迭代器里面必然保存的是某个指针,如果同时使用多种操作去操作链表,
        尤其是迭代器可能会出现大问题,因为迭代器总是要指向某个节点的,如果它指着节点A,而仅仅
        通过自身的方法去操作它,那么不会出问题,但如果它指着节点A,节点A又被其他东西去操作了,例如
        被别的什么东西给delete了,则必然会出问题.

*/


//-------------------------------------------------------------------------------------------------------------------------------

//* 双向链表节点,用于Dlist<T>侵入式链表
//  所有可能作为双向链表的类X,都必须public继承自DlistNode<X>,或public继承自Dlist<X>::node
//  本类主要用于比较基础的操作,若想对链表进行复杂操作,建议直接使用Dlist和它的迭代器
//  注意!本类对象在析构时!不!会!自!动!从所属链表中弹出
template< typename T >
struct DListNode
{
protected:

    DListNode() noexcept : DListNode(this,this) {}

    DListNode(const DListNode&) noexcept :DListNode(){}

    DListNode& operator=(const DListNode&) noexcept { return *this; }

    ~DListNode() =default;

public:

    //* 将本节点从其所属的链表中弹出,如果它本来就不在某个链表中,则什么也不干
    T* setUnlink()
    {
        DListNode<T>* const oldPrev = m_prev;
        DListNode<T>* const oldNext = m_next;
        MemoryFetch( oldNext );
        MemoryFetch( oldPrev );
        m_next = this;
        m_prev = this;
        oldPrev->m_next = oldNext;
        oldNext->m_prev = oldPrev;
        return static_cast<T*>(this);
    }

    void pushNext( T* obj )
    {
        jassert( obj != nullptr );
        jassert( isLinked() || ( !isAvilable() ) ); //你只能向已经连到某条链表中的链表元素连接新元素
        MemoryFetch( obj );
        obj->setUnlink();
        DListNode<T>* oldNext = m_next;
        MemoryFetch( oldNext );
        m_next = obj;
        obj->m_prev = this;
        obj->m_next = oldNext;
        oldNext->m_prev = obj;
    }

    void pushPrev( T* obj )
    {
        jassert( obj != nullptr );
        jassert( isLinked()||(!isAvilable()) ); //你只能向已经连到某条链表中的链表元素连接新元素
        MemoryFetch( obj );
        obj->setUnlink();
        DListNode<T>* oldPrev = m_prev;
        MemoryFetch( oldPrev );
        m_prev = obj;
        obj->m_next = this;
        obj->m_prev = oldPrev;
        oldPrev->m_next = obj;
    }

    T* next() const
    {
        jassert( isLinked() || ( !isAvilable() ) ); //必须在一条链表里,next才有意义!
        MemoryFetch( m_next );
        return static_cast<T*>(m_next);
    }

    T* prev() const
    {
        jassert( isLinked() || ( !isAvilable() ) ); //必须在一条链表里,next才有意义!
        MemoryFetch( m_prev );
        return static_cast<T*>(m_prev);
    }

    bool isLinked() const
    {
        const bool k = m_next != this;
#if JUCE_DEBUG
        if( !k ) {
            jassert( m_prev == this );
        }
#endif
        return k;
    }

    bool isAvilable() const
    {
        return m_avilable;
    }

private:
    DListNode( DListNode<T>* prev , DListNode<T>* next , bool avilable = true ) noexcept
        : m_next( next )
        , m_prev( prev )
        , m_avilable( avilable )
    {
    }

    DListNode<T>* m_next;
    DListNode<T>* m_prev;
    const bool    m_avilable;

    template <typename T2, int place_holder> friend struct DList;

    JUCE_LEAK_DETECTOR(DListNode);
};

//-------------------------------------------------------------------------------------------------------------------------------

//#define ZTD_DLIST_EACH(iterName,listName) for( auto iterName=listName.create_iter(); iterName.isAvailable(); ++iterName )

//* 侵入式双向链表
//  支持两种迭代器: 向后迭代,向前迭代,前后迭代,
//  你可以使用C++11风格的range based for进行链表的遍历,此时,是以正向迭代器做遍历的
//  !! 请绝对注意,当一个双向链表被销毁时,其中的元素!不!会!被!自!动!销!毁! !!
//  这样做的目的是这些元素可能来自于任何地方,例如来自于某个Array.如果你确定当前链表中
//  所有的元素都可以直接被链表所销毁,请手动调用DeleteAll();
template <typename T, int place_holder = 0>
struct DList {

    //* 将DlistNode<T>类重定义为node类
    typedef DListNode<T> NodeType;

    struct Iter {
        
        Iter(T* objToBegin):m_obj(objToBegin) { jassert(objToBegin!=nullptr); };
        
        Iter(const Iter& other) = default;
        
        ~Iter() = default;
        
        //* 向后迭代
        void operator++( ) noexcept{ m_obj = m_obj->next(); }
        
        //* 向前迭代
        void operator--( ) noexcept{ m_obj = m_obj->prev(); }
        
        //* 返回T
        T& operator*( ) const  noexcept{ return *operator->(); }

        //* 返回T
        T* operator->( ) const  noexcept{ jassert( m_obj->isAvilable() ); return static_cast<T*>( m_obj ); }
        
        bool operator!( ) const noexcept{ return m_obj->isAvilable(); }

        //* C++11 range for 兼容
        bool operator!=(const Iter& other) const noexcept { return m_obj!=other.m_obj; }

        //* 在必要时iter可转义为一个T*
        operator T*() const  noexcept{ return operator->(); }
        
        //* 你可以将一整条链表插入到迭代器所指元素的前面
        T* push_prev(T*const k) noexcept{
            m_obj->pushPrev(k);
            return k;
        }
        
        //* 将一个元素插入到到迭代器所指元素的后面
        T* push_next(T*const k) noexcept{
            m_obj->pushNext( k );
            return k;
        }

        //* 你可以将一整条链表插入到迭代器所指元素的前面
        T* push_prev(DList& k,bool returnTailOfK) noexcept{
            unlikely_if( k.is_empty() ) return static_cast<T*>( m_obj );
            NodeType*const k_head = k.m_dummy.m_next;
            NodeType*const k_tail = k.m_dummy.m_prev;
            k._set_empty();
            
            NodeType*const prev = m_obj->m_prev;
            k_head->m_prev = prev;
            k_tail->m_next = m_obj;
            m_obj->m_prev = k_tail;
            prev->m_next = k_head;
            return static_cast<T*>( returnTailOfK ? k_tail : k_head );
        }
        
        //* 你可以将一整条链表插入到迭代器所指元素的后面,k将被清空
        T* push_next(DList& k,bool returnTailOfK ) noexcept{
            unlikely_if( k.is_empty() ) return static_cast<T*>( m_obj );
            NodeType*const k_head = k.m_dummy.m_next;
            NodeType*const k_tail = k.m_dummy.m_prev;
            k._set_empty();
            
            NodeType*const next = m_obj->m_next;
            k_tail->m_next = next;
            k_head->m_prev = m_obj;
            m_obj->m_next = k_head;
            next->m_prev = k_tail;
            return static_cast<T*>(returnTailOfK ? k_tail : k_head);
        }
        
        //* 弹出当前节点,迭代器会自动向右走一格,以便一直指向可用元素
        T* pop_jump_next() noexcept{
            NodeType*const k = m_obj;
            MemoryFetch( m_obj->m_next );
            m_obj = m_obj->m_next;
            k->setUnlink();
            return static_cast<T*>( k );
        }
        
        //* 弹出当前节点,迭代器会自动向左走一格,以便一直指向可用元素
        T* pop_jump_prev() noexcept{
            NodeType*const k = m_obj;
            MemoryFetch( m_obj->m_prev );
            m_obj = m_obj->m_prev;
            k->setUnlink();
            return static_cast<T*>( k );
        }
        
    private:
        NodeType* m_obj;
        friend struct DList;
        void* operator new(size_t) = delete;
        void  operator delete(void*) = delete;
    };

    //* 建立一个空的链表
    DList() noexcept :m_dummy(&m_dummy, &m_dummy,false)
    {
        _set_empty();
        static_assert(std::is_base_of<typename DList<T>::NodeType,T>::value,"T must is a child from DlistNode<T>!");
    }

    //* 释放链表,请注意,这个函数不会删除链表中存在的节点!
    ~DList(){}

    //* 将一整条链表放入当前链表中,另一条链表将被清空
    template<int other_place_holder>
    DList( DList<T,other_place_holder>&& obj ) noexcept
        : DList()
    {
        this->push_tail(obj);
    }

    //* 将一整条链表放入当前链表中,另一条链表将被清空
    template<int other_place_holder>
    DList& operator=( DList<T,other_place_holder>&& obj ) noexcept
    {
        this->push_tail(obj);
        return *this;
    }

    //* 向链表的末尾添加一个节点,如果obj属于另一条链表,则将会先使obj从该条链表中被弹出
    void push_tail( T*const obj ) noexcept {
        jassert(obj!=nullptr);
        obj->setUnlink();
        NodeType*const tail = m_dummy.m_prev;
        tail->pushNext( obj );
    }

    //* 向链表的末尾添加一整条链表,另一条链表将被清空
    template<int other_place_holder>
    void push_tail( DList<T,other_place_holder>& obj ) noexcept {
        unlikely_if(obj.is_empty()) return;
        NodeType*const k_head = obj.m_dummy.m_next;
        NodeType*const k_tail = obj.m_dummy.m_prev;
        obj._set_empty();

        NodeType*const tail = m_dummy.m_prev;
        k_tail->m_next = &m_dummy;
        k_head->m_prev=tail;
        tail->m_next=k_head;
        m_dummy.m_prev = k_tail;
    }

    //* 向链表的头部添加一个节点,如果obj属于另一条链表,将会先使obj从该条链表中被弹出
    void push_head( T*const obj ) noexcept {
        jassert(obj!=nullptr);
        obj->setUnlink();
        NodeType*const head = m_dummy.m_next;
        head->pushPrev( obj );
    }

    //* 向链表的头部添加一整条链表,另一条链表将被清空
    template<int other_place_holder>
    void push_head( DList<T,other_place_holder>& obj ) noexcept {
        unlikely_if(obj.is_empty()) return;
        NodeType*const k_head = obj.m_dummy.m_next;
        NodeType*const k_tail = obj.m_dummy.m_prev;
        obj._set_empty();

        NodeType*const head = m_dummy.m_next;
        k_head->m_prev = &m_dummy;
        k_tail->m_next=head;
        head->m_prev=k_tail;
        m_dummy.m_next = k_head;
    }

    //* 从链表的尾部弹出一个元素,如果链表为空,则返回false
    bool pop_tail( T*& obj ) noexcept {
        NodeType*const tail = m_dummy.m_prev;
        const bool k=!is_empty();
        tail->setUnlink();
        obj = static_cast<T*>( tail );
        return k;
    }

    //* 从链表的头部弹出一个元素,如果链表为空,则返回false
    bool pop_head( T*& obj ) noexcept {
        NodeType*const head = m_dummy.m_next;
        const bool k=!is_empty();
        head->setUnlink();
        obj = static_cast<T*>( head );
        return k;
    }

    //* 检查链表是否为空
    bool is_empty() const noexcept {
        return !getHead()->isAvilable();
    }

    //* 返回链表中head元素
    T* getHead() const noexcept{ return static_cast<T*>( m_dummy.m_next ); }

    //* 返回链表中的tail元素
    T* getTail() const noexcept{ return static_cast<T*>( m_dummy.m_prev ); }

    //* C++11 range for 兼容函数
    Iter begin() const noexcept { return Iter(getHead()); }

    //* C++11 range for 兼容函数
    Iter rbegin() const noexcept{ return Iter(getTail()); }

    //* C++11 range for 兼容函数
    Iter end() noexcept{ return Iter( static_cast<T*>( &m_dummy ) ); }

    //* 立即删除所有链表中的元素,这可能有风险! 请注意插件链表中元素的owner倒是是谁
    void delete_all() noexcept
    {
        for(T* k=nullptr;pop_head(k);delete k);
    }

    //* 使用快排对链表进行排序
    template<typename MaxFunc>
    void sorting(const MaxFunc& doExchangeFun)
    {
        unlikely_if(is_empty()) return;
        DList<T> minList, maxList;
        T* mark = nullptr;
        this->pop_head(mark);
        jassert(mark != nullptr);
        for (T* k; pop_head(k);) {
            if (doExchangeFun(*k, *mark)) {
                maxList.push_head(k);
            }
            else {
                minList.push_head(k);
            }
        }
        maxList.sorting(doExchangeFun); //这里可能会有危险,极端情况可能导致栈溢出
        minList.sorting(doExchangeFun);
        minList.push_tail(mark);
        minList.push_tail(maxList);
        this->push_tail(minList);
    }

    //* 使用牛逼的插入排序对链表进行排序
    template<typename MaxFunc>
    void insertionSortNB(const MaxFunc& doExchangeFun)	//用迭代器做插入排序
    {
        unlikely_if(is_empty()) return;
        Iter forward = this->getHead();		//向前遍历链表，走到头排序完成
        for (++forward; forward!=this->end();) {	//从第二个开始做，检查是否最大
            Iter checker = forward;		//每向前走一个，用来检查已排序链表，查看当前这个是否最大
            for (--checker; checker != this->end() && doExchangeFun(*checker, *forward); --checker) {}
            checker.push_next(forward.pop_jump_next());
        }
    }

    template<typename MaxFunc>
    void mergeWith( DList& otherLsit , const MaxFunc& doExchangeFun )
    {
        unlikely_if( otherLsit.is_empty() ) return; //如果otherList为空,直接退出即可
        Iter b = otherLsit.begin(); //另一个链表的迭代器
        Iter a = this->begin(); //当前链表的迭代器
        jassert( !b ); //b在这里不可能是空的,因为如果是空的,在第一步就该return了
        for( ; ( !a ) && ( !b ); ) { //若a或b已经到头,则退出循环,若a为空,则不进循环
            jassert( !b );
            jassert( !a );
            if( doExchangeFun( *a , *b ) ) {
                a.push_prev( ( b ).pop_jump_next() );//a不动,b进行pop然后向后走一格
            } else {
                ++a; //否则a向后走一格
            }
        }
        this->push_tail( otherLsit );//a或者b在走完循环后可能依然
    }

    template<typename MaxFunc>
    void mergeBucketSorting(const MaxFunc& doExchangeFun) {
        StaticArray<DList<T>, 64> buckets;			//64个桶即64个链表，第0个链表的长度不是0就是1，其他链表的长度不是0就是2^(i-1),所以64个桶可以用来排序2^64个的长度以内的链表，绝对够用了
        for (T* k; this->pop_head(k);) {			//每次弹出第一个节点
            if (!buckets[0].is_empty()) {			//如果第0个桶是空的，就直接把节点丢进去，如果不是空的，就要把桶里的节点往后面的桶里丢，
                for (int i = 0; i < 63; ++i) {		//遍历桶，从前往后归并，如果某一个桶和一个空桶做了归并，就break了
                    bool flag = false;				//是否继续的标志
                    if (buckets[i + 1].is_empty()) flag = true;		//如果后一个桶是空桶，做完这次就跳出循环
                    buckets[i + 1].mergeWith(buckets[i], doExchangeFun);	//i桶和i+1桶做归并
                    if (flag) break;				//跳出循环
                }
            }
            buckets[0].push_tail(k);				//把新来的节点丢到第0个桶里
        }

        for (int i = 0; i < 64; ++i) {
            this->mergeWith(buckets[i], doExchangeFun);		//在把所有桶里的链表再归并到this里
        }
    }

private:
    NodeType m_dummy;

    void _set_empty() noexcept
    {
        m_dummy.m_next = &m_dummy;
        m_dummy.m_prev = &m_dummy;
    }

    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DList);
};

//#define ZTD_DLIST_FOR_EACH(iterName,ListName) for( decltype(ListName)::iterForward iterName=ListName.getHead(); e.isAvailable(); ++e )

//#define ZTD_DLIST_FOR_EACH_BACKWARD(iterName,ListName) for( decltype(ListName)::iterBackward iterName=ListName.getTail(); e.isAvailable(); --e )

}

#endif // ztd_dlistnode_h__
