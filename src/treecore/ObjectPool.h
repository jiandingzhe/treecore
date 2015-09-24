#ifndef TREECORE_OBJECT_POOL_H
#define TREECORE_OBJECT_POOL_H

#include "treecore/AlignedMalloc.h"
#include "treecore/MPL.h"

namespace treecore
{

template<typename T,bool multiThread = true, int allocPreSize = 4*1024>
class ObjectPool
{
private:
        struct EmptySpace: public AlignedMalloc<alignof(T)>
	{
		char data[sizeof( T )];
	};
public:
	typedef typename mpl_type_if<multiThread , LfQueue<RawType*> , Queue<RawType*>>::type QueueType;
public:
	forcedinline ObjectPool(int preAllocBlkNum=1);
	//* 析构函数,会删除这个pool中持有的内存,在执行这个函数之后,从pool中放出的元素则不能再使用
	forcedinline ~ObjectPool();
	forcedinline T* Pop();
	template<typename... InitType>
	forcedinline T* Pop(InitType&... initValues);
	forcedinline void Push(T* k);
    //* 手动创建一些新元素,并加入池中
	forcedinline void CreateSome(int allocBlkNum = 1);
private:
	class ObjBlock
	{
	public:
		ALIGNED_OPERATOR_NEW( ObjBlock , ALIGNOF( T ) );
		ObjBlock();
		~ObjBlock();
		forcedinline RawType* operator[]( const int i );
	private:
		RawType m_elements[allocPreSize];
		TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjBlock);
	};
private:
	typedef typename mpl_type_if<multiThread , LfQueue<ObjBlock*> , Queue<ObjBlock*>>::type ObjGrpList;
private:
	ObjGrpList   m_objGrpList;
	QueueType    m_reuseBuffer;
	TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectPool);
};

//==================================================================================================

template<typename T,bool multiThread,int allocPreSize>
ObjectPool<T,multiThread,allocPreSize>::ObjectPool(int preAllocBlkNum)
	:m_objGrpList(11) //2^11个,够了
	,m_reuseBuffer(17) //2^17个空位置,应该够了吧
{
	//static_assert( IS_TRIVIAL( T ) , "T must be pod!!!" );
	CreateSome(preAllocBlkNum);
}

template<typename T,bool multiThread,int allocPreSize>
ObjectPool<T,multiThread,allocPreSize>::~ObjectPool()
{
	ObjBlock* k;
	likely_while(m_objGrpList.pop(k)) { delete k; }
}

template<typename T,bool multiThread,int allocPreSize>
template<typename... InitType>
T* ObjectPool<T,multiThread,allocPreSize>::Pop(InitType&... initValues)
{
	RawType* k;
	unlikely_while(!m_reuseBuffer.pop(k)) {
		CreateSome();
	}
	T* x = new(k)T(initValues...);
	return x;
}

template<typename T,bool multiThread,int allocPreSize>
T* ObjectPool<T,multiThread,allocPreSize>::Pop()
{
	RawType* k;
	unlikely_while(!m_reuseBuffer.pop(k)) {
		CreateSome();
	}
	T* x= new(k) T();
	return x;
}


template<typename T,bool multiThread,int allocPreSize>
void ObjectPool<T,multiThread,allocPreSize>::Push(T* k)
{
	unlikely_if(k == nullptr) return;
	k->~T();
	m_reuseBuffer.push( (RawType*)(k) ); 
	// 这里应该没问题,为什么呢? 因为k必须是T类型的,而T类型肯定是从这个pool里面放出去的才对,push回来的不可能是个子类,
	// 即使是子类,也已经在push的时候转回T了
}


template<typename T,bool multiThread,int allocPreSize>
void ObjectPool<T,multiThread,allocPreSize>::CreateSome(int allocBlkNum)
{
	jassert( allocBlkNum>0 );
	int i=0;
	do {
		ObjBlock*const m = new ObjBlock;
		m_objGrpList.push(m);
		for(int i = 0; i < allocPreSize; ++i) {
			m_reuseBuffer.push( (*m)[i] );
		}
	} unlikely_while( ++i < allocBlkNum );
}

template<typename T,bool multiThread,int allocPreSize>
typename ObjectPool<T , multiThread , allocPreSize>::RawType* ObjectPool<T , multiThread , allocPreSize>::ObjBlock::operator[]( const int i )
{
	return &m_elements[i];
}

template<typename T,bool multiThread,int allocPreSize>
ObjectPool<T,multiThread,allocPreSize>::ObjBlock::ObjBlock()
{}


template<typename T,bool multiThread,int allocPreSize>
ObjectPool<T,multiThread,allocPreSize>::ObjBlock::~ObjBlock()
{}

//============================================================================================

template<typename T , bool multiThread = true , int allocNum = 4 * 1024>
class CreateFromPool : public SharedSingleton< ObjectPool<T , multiThread , allocNum> >::EarlyUser
{
private:
    typedef SharedSingleton< ObjectPool<T , multiThread , allocNum> > CreatePool;
public:
    static forcedinline void CreateSome(int blkNum=1)
    {
        CreatePool::getInstance().CreateSome(blkNum);
    }
    static forcedinline T* alloc()
	{
        return getInstance().Pop();
	};
    template<typename... InitType>
    static forcedinline T* alloc( InitType&... initTypes )
    {
        return getInstance().Pop( initTypes... );
    };
	static forcedinline void free( T* ptr )
	{
        return getInstance().Push(ptr);
	};
protected:
    TRIVIAL_CLASS(CreateFromPool);
};

} // namespace treecore

#endif // TREECORE_OBJECT_POOL_H
