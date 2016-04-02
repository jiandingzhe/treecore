//!********************************************************************************
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! @file    ztd_DlistNode.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2015/11/04
//! @brief   
//!********************************************************************************

#ifndef ____ZTD_DLIST__E98C9B7A_DB97_4D28_B521_D85F59F65D91
#define ____ZTD_DLIST__E98C9B7A_DB97_4D28_B521_D85F59F65D91


namespace juce
{

enum DlistDeleteOpertaion
{
    listAutoDeleteNodes,
    listKeepNodeLinked,
    listDoNothing
};

template< 
    bool CountMember ,                     //< 链表将进行计数以便知道自己内部的元素个数
    DlistDeleteOpertaion deleteOperataion, // 链表在删除时会自动删除自己内部的元素
    bool holdMyList                        //链表元素中持有着自己所在的链表
>
struct DlistDescription
{

};

template< typename T , int multi_dim_place_holder = 0 >
class Dlist
{

};



//=================================================================================================================================================================================================================================================================================================================================================

/*
    作为一个C+++风格的侵入式链表,我们有三种方法去操作一个链表:

	1.通过节点操作
	2.通过链表类本身操作
	3.通过迭代器操作

	一般来说,3符合现代C++11风格,但是,它是有局限性的,
	总会有时候你需要直接操作节点的,又总会有需要直接在链表
	上操作的. 有一些库只使用1的方式操作链表,那些库只提供
	链表节点类,而不提供链表类或者迭代器类.这样做是可以的,
	但是这样做的代价是缺乏一个标示一整条链表的能力,例如:

	A<->B<->C<->D

	如果我们用某种方法,例如一个指针指向A,以此标志ABCD这条
	链表,那么可能出现的问题是如果在之后将A弹出并删除了,则
	这个链表标示会失效,因此必须使用某个机制使得当A失效时
	这个标示自动转向B.这会带来麻烦.

	另一种选择,即只能操作链表或迭代器,不能直接操作链表元素,
	这依然会带来麻烦例如当前我只知道某个链表元素,甚至当前
	函数就在链表元素的内部,我根本以简单的方式找到这个元素
	所处的链表是哪条,于是完全无法操作这个元素,当然我们可以
	为每一个元素加一个成员指向这个元素所处的链表,但这样做就
	无法简单地将整条链表插入到另一个链表里了.我认为整条链表
	这样操作是很有必要的.如果放弃以单个元素操作整个链表的能力,
	那么和std::list有什么区别?

	那么,123这三种操作都允许行不行呢? 也危险,迭代器里面总是
	要指向某个节点的,如果它指着节点A,而仅仅通过自身的方法去
	操作它,那么不会出问题,但如果它指着节点A,节点A又被其他东
	西去操作了,例如被别的什么东西给set_unlink了,则迭代器立即
	就会失效.

	那么,怎么办?

	以下有三个类,分别是链表元素,迭代器和链表. 链表元素使用
	典型的双向链表.链表结构如下:

	+--<-> A <-> B <-> C <-> C <->--+
	|                               |
	+-------- <-> dummy <-> --------+
	
	当链表元素未连接任何链表时,结构如下:

	+->A<-+
	|  |  |
	+-----+
	
    +->A<->B<->+
    |          |
    +----------+
	
	* 使用head和tail两个dummy node的链表有一个问题,即
	  如果迭代器支持双向迭代,那么每一次都既要检测节点
	  是不是dummy head,又要检测节点是不是dummy tail
	  所以最后不得已,还是加入了一个bool值成员,判断这个节点
	  是不是一个有效的节点,还是只是个dummy

	使用Dlist时,提供123三种方法使用,但有以下限制:

	1.链表元素可直接插入删除操作,但进行操作时必须
	  保证链表元素已经在某个链表中了! 换句话说,
	  你不能在一个自由的链表元素上插入其他元素构成
	  链表(虽然按道理说是可以的).
	  只有当一个链表元素已经属于某个链表了,这么做才有意义.

	2. 迭代器,链表元素和链表本身均提供各种链表操作,但使用
	   迭代器的时候和链表元素本身的pop时要小心.当一个迭代器
	   存在时,请使用迭代器的pop而不是链表元素本身提供的set_unlink()

	这样带来的好处:

	1. 侵入式链表
	2. 一次插入一整条链表
	3. 三种操作方式,基本随意操作

	为什么要搞一个bool值,而不是搞其他的,比如头尾指针呢? 因为例如:

	for( Dlist<A>::Iter e = list1.begin(); e != list1.end(); ++e ) {
	    list[k].push_tail( e.pop_jump_prev() );
	}
	
	如果没有bool值,就需要狂检测.检查头,检查尾,检查头尾.......



    +---- A ----+
    |           |
    +-- Empty --+

    +---- B ----+
    |           |
    +-- Empty --+

    +---- A ---- B ----+
    |                  |
    +------ Empty -----+
    
    +-- Dummy --+
    |           |
    +-----------+

    +-- A --+
    |       |
    +-------+

    +-- Dummy --+
    |           |
    +---- A ----+

    +-- Dummy ---+
    |            |
    +-- A -- B --+

*/


template< typename T , int multi_dim_place_holder = 0 >

struct Dlist;

template< typename T , int multi_dim_place_holder = 0 >
struct DlistIter;


//* 双向链表节点,用于Dlist<T>侵入式链表
//  所有可能作为双向链表的类X,都必须public继承自DlistNode<X>,或public继承自Dlist<X>::node
//  本类主要用于比较基础的操作,若想对链表进行复杂操作,建议直接使用Dlist和它的迭代器
//  注意!本类对象在析构时!不!会!自!动!从所属链表中弹出
//  
//  multi_dim_place_holder 的作用,只有当使用十字链表或更多维链表时才有用,
//  当你需要一个类是个十字链表时,你需要继承好几次DlistNode
//  然而一个类只能继承同一个基类一次,这里就用到multi_dim_place_holder了,
//  由于C++规定不同模板参数的两个模板特化不是同一个类,所以
//  你只要使用不同的multi_dim_place_holder,如0,1,2,3,4,就能让一个类
//  继承好几次DlistNode从而制作十字链表或更多维度的链表了.
template< typename T , int multi_dim_place_holder = 0 >
struct DlistNode 
{
	typedef Dlist<T , multi_dim_place_holder> List;
	typedef DlistIter<T , multi_dim_place_holder> Iter;

private:
    typedef DlistNode<T , multi_dim_place_holder> Node;

protected:

	forcedinline DlistNode() noexcept
        : DlistNode(true)
    {
    };

    forcedinline DlistNode( const DlistNode& ) noexcept
        : DlistNode()
    {
    };

    forcedinline DlistNode& operator=( const DlistNode& ) noexcept
    {
        return *this;
    };

	forcedinline ~DlistNode() noexcept
    {
        jassert( !m_is_available || m_empty_ptr == empty() );
		set_unlink();
    };

public:

    //! @brief  将本节点从其所属的链表中弹出,如果它本来就不在某个链表中,则什么也不干 
	inline T* set_unlink() noexcept
    {
        auto* const oldPrev = m_prev;
        auto* const oldNext = m_next;
        MemoryFetch( oldNext );
        MemoryFetch( oldPrev );
        _switch_to_empty_link();
        _unsafe_set_unlink();
        oldPrev->m_next = oldNext;
        oldNext->m_prev = oldPrev;
        return static_cast<T*>( this );
    };

    //! @brief 将节点插入到当前节点之后,若obj节点本身
	//!        已经属于某条链表,则其会先从那条链表中弹出
	//! @warning 必须在本节点属于某个链表时才能调用
    inline void insert_next( T* k ) noexcept
    {
        Node* const obj = k;
        jassert( obj != nullptr );
        jassert( obj != this ); //等于自己并不会破坏逻辑,但你确定这样对吗..
        MemoryFetch( obj );
        obj->set_unlink();
        auto* const oldNext = m_next;
        MemoryFetch( oldNext );
        m_next = obj;
        obj->m_prev = this;
        obj->m_next = oldNext;
        oldNext->m_prev = obj;
    };

    //! @brief 将节点插入到当前节点之前,若obj节点本身
	//!        已经属于某条链表,则其会先从那条链表中弹出
	//! @warning 必须在本节点属于某个链表时才能调用
    inline void insert_prev( T* k ) noexcept
    {
        Node* const obj = k;
        jassert( obj != nullptr );
        jassert( obj != this ); //等于自己并不会破坏逻辑,但你确定这样对吗..
        MemoryFetch( obj );
        obj->DlistNode<T , multi_dim_place_holder>::set_unlink();
        auto* const oldPrev = m_prev;
        MemoryFetch( oldPrev );
        m_prev = obj;
        obj->DlistNode<T , multi_dim_place_holder>::m_next = this;
        obj->DlistNode<T , multi_dim_place_holder>::m_prev = oldPrev;
        oldPrev->m_next = obj;
    };

    //! @brief 将整条链表插入到当前节点之后,otherList将被清空
	//! @return  ?
	//! @warning 必须在本节点属于某个链表时才能调用
	T* insert_next( List&& otherList , bool returnTailOfOther = true ) noexcept
    {
        unlikely_if( otherList.is_empty() ) return static_cast<T*>( this );

        auto* const otherList_head = otherList.m_dummy.m_next;
        auto* const otherList_tail = otherList.m_dummy.m_prev;
        otherList._unsafe_set_empty();

        auto* const next = this->m_next;
        otherList_tail->m_next = next;
        otherList_head->m_prev = this;
        this->m_next = otherList_head;
        next->m_prev = otherList_tail;
        return static_cast<T*>( returnTailOfOther ? otherList_tail : otherList_head );
    };
	forcedinline T* insert_next( List&  otherList , bool returnTailOfOther = true ) noexcept
    {
        return insert_next( std::move( otherList ) , returnTailOfOther );
    };

    //! @brief  将整条链表插入到当前节点之前,otherList将被清空
	//! @return ?  
	//! @warning 必须在本节点属于某个链表时才能调用
	T* insert_prev( List&& otherList , bool returnTailOfOther = true ) noexcept
    {
        unlikely_if( otherList.is_empty() ) return static_cast<T*>( this );

        auto* const otherList_head = otherList.m_dummy.m_next;
        auto* const otherList_tail = otherList.m_dummy.m_prev;
        otherList._unsafe_set_empty();

        auto* const prev = this->m_prev;
        otherList_head->m_prev = prev;
        otherList_tail->m_next = this;
        this->m_prev = otherList_tail;
        prev->m_next = otherList_head;
        return static_cast<T*>( returnTailOfOther ? otherList_tail : otherList_head );
    };
	forcedinline T* insert_prev( List&  otherList , bool returnTailOfOther = true ) noexcept
    { 
        return insert_prev( std::move( otherList ) , returnTailOfOther );
    };

    //! @brief  返回当前节点的后一个节点
	//! @warning 必须在本节点属于某个链表时才能调用
	forcedinline T* next() const noexcept
	{
        auto* const k = m_next;
        MemoryFetch( k );
        return k->m_is_available ? static_cast<T*>( k ) : nullptr;
    };
    
    //! @brief  返回当前节点的前一个节点
	//! @warning 必须在本节点属于某个链表时才能调用
	forcedinline T* prev() const noexcept
	{
        auto* const k = m_prev;
        MemoryFetch( k );
        return k->m_is_available ? static_cast<T*>( k ) : nullptr;
    };

    //! @brief   返回当前节点是否属于某个链表
	//! @warning 如果当前节点并不属于哪个链表,但其与另几个节点已经连接,
	//!          则这个函数也会返回true
    forcedinline bool is_linked() const noexcept
    {
        return m_next != empty() || m_prev != empty();
    };

private:
    Node*      m_empty_ptr;
    Node*      m_next;
    Node*      m_prev;
    const bool m_is_available;

    //! \brief 用于当前节点自身的unlink,并不修改其他节点与当前节点的连接性
    //!        所以是"unsafe"
	forcedinline void _unsafe_set_unlink() noexcept
    {
        m_next = m_empty_ptr;
        m_prev = m_empty_ptr;
    };

    //! \brief  node will self link when set_unlink();
    forcedinline void _switch_to_self_link() noexcept
    {
        m_empty_ptr = this;
    };

    //! \brief node will empty link when set_unlink();
    forcedinline void _switch_to_empty_link() noexcept
    {
        m_empty_ptr = m_is_available ? empty() : this;
    };

    //! \brief 构造的同时传入一个布尔值控制当前的节点是否是个有效节点
    DlistNode( bool is_available ) noexcept
        : m_empty_ptr( is_available ? empty() : this )
        , m_is_available( is_available )
    {
        _unsafe_set_unlink();
    };

    //! \brief 一个全局的空节点,避免操作nullptr时带来的判断
    static forcedinline Node* empty() noexcept
    {
        static DlistNode k( false );
        return &k;
    };

    template< typename T2 , int multi_dim_place_holder2 > friend struct DlistIter;
    template< typename T2 , int multi_dim_place_holder2 > friend struct Dlist;
    JUCE_LEAK_DETECTOR( DlistNode );
};


//! 控制迭代器的在插入整条链表时的jump行为,@see DlistIter
enum JumpState
{
    jump_to_tail = 0 ,
    jump_to_head = 1 ,
    stay_there = 2
};


template< typename T , int multi_dim_place_holder >
struct DlistIter
{
    typedef Dlist<T , multi_dim_place_holder> List;
    typedef DlistNode<T , multi_dim_place_holder> Node;


    //! ----------------------------------------------------
    //! @brief 拷贝构造,非常轻量级 
    DlistIter( const DlistIter& node ) = default;


    //! ----------------------------------------------------
    //! @brief 拷贝,非常轻量级
    DlistIter& operator=( const DlistIter& other ) = default;


    //! ----------------------------------------------------
    //! @brief 什么都不做的析构
    ~DlistIter() = default;


    //! ----------------------------------------------------
    //! @brief C++11 range based for 兼容函数,用来对比两个
    //!        迭代器是否不相等
    forcedinline bool operator!=( const DlistIter& node ) const noexcept
    {
        return m_obj != node.m_obj;
    };

    forcedinline bool operator!( ) const noexcept
    {
        return m_obj->m_is_available;
    };

    //! ----------------------------------------------------
    //! @brief  向后迭代
    forcedinline void operator ++ ( ) noexcept
    {
        m_obj = m_obj->DlistNode<T , multi_dim_place_holder>::m_next;
    };


    //! ----------------------------------------------------
    //! @brief   向前迭代
    forcedinline void operator -- ( ) noexcept
    {
        m_obj = m_obj->DlistNode<T , multi_dim_place_holder>::m_prev;
    };


    //! ----------------------------------------------------
    //! @brief   将一个元素或链表插入到迭代器所指的元素前面
    //! @return  返回你插入的那个元素
    forcedinline void insert_prev( T*const k ) noexcept
    {
        k->DlistNode<T , multi_dim_place_holder>::m_empty_ptr = k;

        m_obj->DlistNode<T , multi_dim_place_holder>::insert_prev( k );
    };


    //! ----------------------------------------------------
    //! @brief   将一整条链表插入到迭代器所指元素的前面.链表k会自动清空
    //! @param   returnTailOfK 为true时返回k的尾巴,为false返回k的头,如果k
    //!          是空的则返回当前迭代器指向的那个元素.
    //! @param   jumpState 为jump_to_tail时,迭代器执行完函数后将自动偏移到
    //!          链表k的尾部,为jump_to_head时,迭代器将偏移到k的头部,stay_there
    //!          时将保持位置不变.这个选项的目的是如果你在遍历链表,迭代器当前
    //!          位置你插入了一个元素,那么如果迭代器位置保持不变的话,下一次前进
    //!          将前进到刚刚插入的队列的第一个元素上,这可能是你想要的结果也可能不是,
    //!          因此有这么个选项.
    forcedinline void insert_prev( List&& k , JumpState jumpState = stay_there ) noexcept
    {
        auto* a = m_obj->DlistNode<T , multi_dim_place_holder>::insert_prev( k , jumpState == jump_to_tail );
        m_obj = jumpState == stay_there ? m_obj : a;
    };

    forcedinline void insert_prev( List& k , JumpState jumpState = stay_there ) noexcept
    {
        insert_prev( std::move( k ) , jumpState );
    };


    //! ----------------------------------------------------
    //! @brief   将一个元素插入到到迭代器所指元素的后面
    //! @return  返回你插入的那个元素
    forcedinline void insert_next( T*const k ) noexcept
    {
        k->DlistNode<T , multi_dim_place_holder>::m_empty_ptr = k;

        m_obj->DlistNode<T , multi_dim_place_holder>::insert_next( k );
    };


    //! ----------------------------------------------------
    //! @brief   将一整条链表插入到迭代器所指元素的后面.链表k会自动清空
    //! @param   returnTailOfK 为true时返回k的尾巴,为false返回k的头,如果k
    //!          是空的则返回当前迭代器指向的那个元素.
    //! @param   jumpState 为jump_to_tail时,迭代器执行完函数后将自动偏移到
    //!          链表k的尾部,为jump_to_head时,迭代器将偏移到k的头部,stay_there
    //!          时将保持位置不变.这个选项的目的是如果你在遍历链表,迭代器当前
    //!          位置你插入了一个元素,那么如果迭代器位置保持不变的话,下一次前进
    //!          将前进到刚刚插入的队列的第一个元素上,这可能是你想要的结果也可能不是,
    //!          因此有这么个选项.
    forcedinline void insert_next( List&& k , JumpState jumpState = jump_to_tail ) noexcept
    {
        auto* a = m_obj->DlistNode<T , multi_dim_place_holder>::insert_next( k , jumpState == jump_to_tail );
        m_obj = jumpState == stay_there ? m_obj : a;
    };

    forcedinline void insert_next( List& k , JumpState jumpState = jump_to_tail ) noexcept
    {
        insert_next( std::move( k ) , jumpState );
    };


    //! ----------------------------------------------------
    //! @brief 弹出当前节点,迭代器会自动向右走一格,以便一直指向可用元素
    //! @warning 会!自!动!向!右!走!一!格!
    forcedinline T* pop_jump_next() noexcept
    {
        Node*const k = m_obj;
        MemoryFetch( m_obj->DlistNode<T , multi_dim_place_holder>::m_next );
        m_obj = m_obj->DlistNode<T , multi_dim_place_holder>::m_next;

        k->DlistNode<T , multi_dim_place_holder>::_switch_to_empty_link();

        k->DlistNode<T , multi_dim_place_holder>::set_unlink();
        return static_cast<T*>( k );
    };


    //! ----------------------------------------------------
    //! @brief 弹出当前节点,迭代器会自动向左走一格,以便一直指向可用元素  
    //! @warning 会!自!动!向!左!走!一!格!
    forcedinline T* pop_jump_prev() noexcept
    {
        Node*const k = m_obj;
        MemoryFetch( m_obj->DlistNode<T , multi_dim_place_holder>::m_prev );
        m_obj = m_obj->DlistNode<T , multi_dim_place_holder>::m_prev;

        k->DlistNode<T , multi_dim_place_holder>::_switch_to_empty_link();

        k->DlistNode<T , multi_dim_place_holder>::set_unlink();
        return static_cast<T*>( k );
    };


    //! ----------------------------------------------------
    //! @brief 返回T   
    //! @warning 调用前务必确认迭代器有效!
    forcedinline T& operator * ( ) const  noexcept
    {
        return *this->operator->( );
    };

    forcedinline T* operator -> ( ) const  noexcept
    {
        return static_cast<T*>( m_obj );
    };


private:
    Node* m_obj;

    void* operator new( size_t ) = delete;
    void  operator delete( void* ) = delete;

    friend struct DlistNode<T , multi_dim_place_holder>;
    friend struct Dlist<T , multi_dim_place_holder>;

    // 私有! 别随便用
    DlistIter() = default;

    forcedinline DlistIter( Node* obj ) noexcept : m_obj( obj ) {};

    JUCE_LEAK_DETECTOR( DlistIter );
};



template <typename T , int multi_dim_place_holder >
struct Dlist
{
    //* 将DlistNode<T>类重定义为node类
    typedef DlistNode<T , multi_dim_place_holder> Node;
    typedef DlistIter<T , multi_dim_place_holder> Iter;

    //! @brief  建立一个空的链表
    forcedinline Dlist() noexcept	
        : m_dummy( false )
    {
    };

    //! @brief  将一整条链表放入当前链表中,另一条链表将被清空
    forcedinline Dlist( Dlist&& obj ) noexcept	
        : Dlist()
    {
        ( *this ) = std::move(obj);
    };

    //! @brief 将一整条链表放入当前链表中,另一条链表将被清空
    forcedinline Dlist& operator=( Dlist&& obj ) noexcept
    {
        push_tail( obj );
        return *this;
    };

    //! @brief  析构,什么都不做
    //! @warning 这个函数 不! 会! 删! 除! 链表中存在的节点!!
    forcedinline ~Dlist() noexcept
    {
        //以下代码比较hack
        for( auto e = begin(); !e;++e )
        {
            e->DlistNode<T , multi_dim_place_holder>::_switch_to_empty_link();
        }
        DlistNode<T , multi_dim_place_holder>* const head = m_dummy.m_next;
        DlistNode<T , multi_dim_place_holder>* const tail = m_dummy.m_prev;
        head->m_next = head->m_empty_ptr;
        tail->m_next = tail->m_empty_ptr;
        m_dummy._unsafe_set_unlink();
    };

    //! @brief   向链表的末尾添加一个节点或一整条链表,如果obj
    //!          属于另一条链表,则将会先使obj从该条链表中被弹出
    //! @param   obj: 你要加进去的节点或链表 
    //! @warning 就问你爽不爽,就问你拿stl::list怎么搞?
    forcedinline void push_tail( T*const obj ) noexcept
    {
        obj->DlistNode<T , multi_dim_place_holder>::_switch_to_self_link();

        m_dummy.DlistNode<T , multi_dim_place_holder>::insert_prev( obj );
    };

    forcedinline void push_tail( Dlist&& otherList ) noexcept
    {
        m_dummy.DlistNode<T , multi_dim_place_holder>::insert_prev( otherList );
    };

    forcedinline void push_tail( Dlist& otherList ) noexcept
    {
        push_tail( std::move( otherList ) ); 
    };

    //! @brief   向链表的头部添加一个节点或一整条链表,如果obj
    //!          属于另一条链表,则将会先使obj从该条链表中被弹出
    //! @param   obj: 你要加进去的节点或链表 
    //! @warning 就问你爽不爽,就问你拿stl::list怎么搞?
    forcedinline void push_head( T*const obj ) noexcept
    {
        obj->DlistNode<T , multi_dim_place_holder>::_switch_to_self_link();

        m_dummy.DlistNode<T , multi_dim_place_holder>::insert_next( obj );
    };

    forcedinline void push_head( Dlist&& otherList ) noexcept
    {
        m_dummy.DlistNode<T , multi_dim_place_holder>::insert_next( otherList );
    };

    forcedinline void push_head( Dlist& otherList ) noexcept
    {
        push_head( std::move( otherList ) ); 
    };

    //! @brief  从链表的尾部弹出一个元素,如果链表为空,则返回false 
    forcedinline bool pop_tail( T*& obj ) noexcept
    {
        auto* const k = m_dummy.DlistNode<T , multi_dim_place_holder>::m_prev;

        k->DlistNode<T , multi_dim_place_holder>::_switch_to_empty_link();

        k->DlistNode<T , multi_dim_place_holder>::set_unlink();
        obj = static_cast<T*>( k );
        return k != &m_dummy;
    };

    //! @brief  从链表的头部弹出一个元素,如果链表为空,则返回false 
    forcedinline bool pop_head( T*& obj ) noexcept
    {
        auto* const k = m_dummy.DlistNode<T , multi_dim_place_holder>::m_next;

        k->DlistNode<T , multi_dim_place_holder>::_switch_to_empty_link();

        k->DlistNode<T , multi_dim_place_holder>::set_unlink();
        obj = static_cast<T*>( k );
        return k != &m_dummy;
    };

    //! @brief  检查链表是否为空
    forcedinline bool is_empty() const noexcept
    {
        jassert( 
            ( m_dummy.m_next == &m_dummy && m_dummy.m_prev == &m_dummy ) 
         || ( m_dummy.m_next != &m_dummy && m_dummy.m_prev != &m_dummy ) );
        return m_dummy.m_next == &m_dummy;
    };

    //! @brief 返回链表中head元素
    //! @warning 调用此函数时必须保证链表不是空的,否则行为未定义
    forcedinline T* get_head() const noexcept
    {
        return m_dummy.DlistNode<T , multi_dim_place_holder>::next();
    };

    //! @brief 返回链表中的tail元素
    //! @warning 调用此函数时必须保证链表不是空的,否则行为未定义
    forcedinline T* get_tail() const noexcept
    {
        return m_dummy.DlistNode<T , multi_dim_place_holder>::prev();
    };

    //! @brief   从当前链表弹出迭代器,正向或者反向(不加r或者加r).
    //!          使用for( Iter e=x.begin();e!=e.end();++e){}
    //!          的形式遍历.这几个函数的行为符合 C++11 range for 
    //!          的定义,可以直接写 for(auto e:x){}.
    //! @return  返回迭代器, @see DlistIter.
    //! @warning 迭代器本身可以正向前进和反向前进,如果正反向
    //!          来回前进,必须做双向检查其碰触end和rend.
    forcedinline const Iter begin() const noexcept
    {
        return Iter( const_cast<Node*>( m_dummy.DlistNode<T , multi_dim_place_holder>::m_next ) );
    };

    forcedinline const Iter rbegin() const noexcept
    {
        return Iter( const_cast<Node*>( m_dummy.DlistNode<T , multi_dim_place_holder>::m_prev ) );
    };

    forcedinline const Iter end() const noexcept
    {
        return Iter( const_cast<Node*>( &m_dummy ) );
    };

    forcedinline const Iter rend() const noexcept
    {
        return Iter( const_cast<Node*>( &m_dummy ) );
    };

    //! @brief 立刻删除链表中的所有元素
    //! @warning 本函数有风险! 它更像手动管理内存时代的产物.
    //!          请务必注意链表中元素的owner倒是是谁,如果不是
    //!          本链表,则这个函数的调用会引发错误.
    inline void delete_all() noexcept
    {
        for( auto e = begin(); e != end(); delete e.pop_jump_next() );
    };

    //! @brief   下列函数们用来将一条链表塞入本条已经排好序的链表之中
    //!          排序原则是小的节点放在前面,大的放在后面.
    //!          即若a>b,则b放前面,a放后面
    //! @param   otherList 要放入的那条链表.
    //! @param   doExchangeFun 若T类型提供了">"操作符,则可以不填写这个
    //!                        参数,而是使用">",否则必须提供本参数
    //! @warning 当前链表必须是排好序的! 否则行为未定义!
    template<typename MaxFunc>
    inline void ordered_merge( Dlist&& otherList , const MaxFunc& returnTrueIfUWantExchange ) noexcept
    {
        unlikely_if( otherList.is_empty() ) return;            //如果otherList为空,直接退出即可
        Iter b = otherList.begin();                            //另一个链表的迭代器
        Iter a = this->begin();                                //当前链表的迭代器
        for( ; ( a != end() ) && ( b != otherList.end() ); ) { //若a或b已经到头,则退出循环,若a为空,则不进循环
            if( returnTrueIfUWantExchange( *a , *b ) ) {
                a.insert_prev( ( b ).pop_jump_next() ); //a不动,b进行pop然后向后走一格
            } else {
                ++a; //否则a向后走一格
            }
        }
        this->push_tail( otherList );//a或者b在走完循环后可能依然
    };
    template<typename MaxFunc>
    forcedinline void ordered_merge( Dlist& otherList , const MaxFunc& returnTrueIfUWantExchange ) noexcept
    {
        ordered_merge( std::move( otherList ) , returnTrueIfUWantExchange ); 
    };
    forcedinline void ordered_merge( Dlist& otherList ) noexcept
    {
        ordered_merge( std::move( otherList ) ); 
    };
    forcedinline void ordered_merge( Dlist&& otherList ) noexcept
    {
        ordered_merge( otherList , [&]( const T& a , const T& b ){ return a > b; } );
    };

    //! @brief  下列函数们用来对当前链表进行排序,排序原则是
    //!         小的节点放前面,大的放后面.
    //! @param   doExchangeFun 若T类型提供了"<"操作符,则可以不填写这个
    //!                        参数,而是使用"<",否则必须提供本参数.
    //! @warning 排序速度快! 真汉子!
    //TODO 如需传doExchangeFun 使用起来有迷惑 不知道return a < b 和return a > b  到底代表正序还是倒序
    template<typename MaxFunc>
    void sort( const MaxFunc& doExchangeFun ) noexcept
    {
        static_array<Dlist , 64> buckets;		//64个桶即64个链表，第0个链表的长度不是0就是1，其他链表的长度不是0就是2^(i-1),所以64个桶可以用来排序2^64个的长度以内的链表，绝对够用了
        for( T* k; this->pop_head( k ); ) {			//每次弹出第一个节点
            if( !buckets[0].is_empty() ) {			//如果第0个桶是空的，就直接把节点丢进去，如果不是空的，就要把桶里的节点往后面的桶里丢，
                for( int i = 0; i < 63; ++i ) {		//遍历桶，从前往后归并，如果某一个桶和一个空桶做了归并，就break了
                    bool flag = false;				//是否继续的标志
                    if( buckets[i + 1].is_empty() ) flag = true;		//如果后一个桶是空桶，做完这次就跳出循环
                    buckets[i + 1].ordered_merge( buckets[i] , doExchangeFun );	//i桶和i+1桶做归并
                    if( flag ) break;				//跳出循环
                }
            }
            buckets[0].push_tail( k );				//把新来的节点丢到第0个桶里
        }

        for( int i = 0; i < 64; ++i ) {
            this->ordered_merge( buckets[i] , doExchangeFun ); //在把所有桶里的链表再归并到this里
        }
    };

    //! @brief  if class T already has ">"operator , than you can use this 
    //!         method to sort without a lambda. 
    forcedinline void sort() noexcept
    {
        sort( []( const T& a , const T& b ){ return a > b; } );
    };


    //! @brief  下列函数用来检查链表是否已经处于排序状态
    //! @param   doExchangeFun 若T类型提供了"<"操作符,则可以不填写这个
    //!                        参数,而是使用"<",否则必须提供本参数.
    template<typename MaxFunc>
    bool check_sorted( const MaxFunc& doExchangeFun ) const noexcept
    {
        auto e = this->begin();
        auto olde = e;
        for( ++e; e != this->end(); ++e ) {
            if( doExchangeFun( *olde , *e ) ) {
                return false;
            }
            olde = e;
        }
        return true;
    };
    forcedinline bool check_sorted() const noexcept
    {
        return check_sorted( []( const T& a , const T& b ){ return a > b; } );
    };


private:
    Node m_dummy;
    Node* dummyPtr = &m_dummy;

    template <typename T2 , int multi_dim_place_holder_foo>
    friend struct DlistIter;

    template <typename T2 , int multi_dim_place_holder_bar>
    friend struct DlistNode;

    forcedinline void _unsafe_set_empty() noexcept
    {
        m_dummy._unsafe_set_unlink();
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Dlist );
};


}

#endif // ____ZTD_DLISTNODE__E98C9B7A_DB97_4D28_B521_D85F59F65D91
