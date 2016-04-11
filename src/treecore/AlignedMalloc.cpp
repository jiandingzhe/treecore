#include "treecore/AlignedMalloc.h"
#include "treecore/CheckedMalloc.h"
#include "treecore/MathsFunctions.h"
#include "treecore/Memory.h"

namespace treecore
{

struct AlignedMemClip {
	int offset;
	size_t len;
	
        static forcedinline AlignedMemClip* CreateFromRawMem(size_t const len,size_t const algn_val_ispw2)
	{
		const size_t real_len_byte = len + sizeof(AlignedMemClip) + algn_val_ispw2;
		char* ptr = (char*)checked_malloc(real_len_byte);
                if unlikely(ptr == nullptr) throw std::bad_alloc();
		size_t const algn_mask = algn_val_ispw2 - 1;
		char* ptr2 = (char*)ptr + sizeof(AlignedMemClip); //定位最大偏移余量指针
		char*const algnd_ptr = ptr2 + ( algn_val_ispw2 - ( (size_t)ptr2 & algn_mask ) ); //设置对齐了的指针
		AlignedMemClip* p = (AlignedMemClip*)(algnd_ptr - sizeof(AlignedMemClip)); //我们就不从rawPtr开始了,而是从alignedptr开始向前推
		new(p) AlignedMemClip;
		p->len=len;
		p->offset=(int)( algnd_ptr - ptr ); //将offset写入向前推的那个int中
		return p;
	}
    
        static forcedinline AlignedMemClip* GetMemClipFromAlignedPtr(void* ptr) noexcept
	{
		AlignedMemClip* p = (AlignedMemClip*)((char*)ptr-sizeof(AlignedMemClip));
		return p;
	}
    
	forcedinline void* GetAlignedPtr() const noexcept { return (char*)this+sizeof(AlignedMemClip); }
    
	forcedinline void* GetRawPtr() const noexcept { return (char*)GetAlignedPtr()-offset; }
};

//* 分配对齐的内存
void* aligned_malloc(size_t length_byte_0,size_t const algn_val_ispw2)
{
        if unlikely(length_byte_0 == 0) return nullptr; //这里跟标准的malloc有点不一样
	checkPowerOfTwo(algn_val_ispw2);
    
	AlignedMemClip* const clip = AlignedMemClip::CreateFromRawMem(length_byte_0,algn_val_ispw2);
    
	void* alp= clip->GetAlignedPtr();
	checkPtrSIMD(alp,algn_val_ispw2);
	return alp;
}

//* 分配对齐的内存,并将值清零
void* aligned_calloc(size_t length_byte_0,size_t const algn_val_ispw2)
{
	void* const p = aligned_malloc(length_byte_0,algn_val_ispw2);
	if likely(p != NULL) zeromem(p,length_byte_0);
	return p;
}

//* 释放对齐的内存,必须和aligned_malloc或者aligned_calloc一起使用,不能用来释放malloc的内存
void aligned_free(void*const ptr_a) noexcept
{
	if unlikely(ptr_a == nullptr) return;
	checked_free( AlignedMemClip::GetMemClipFromAlignedPtr(ptr_a)->GetRawPtr() );
}

//* 分配对齐的内存,并将值清零
void* aligned_realloc(void* ptr,size_t length_byte_0,size_t const algn_val_ispw2)
{
	if unlikely(ptr == nullptr) return aligned_malloc(length_byte_0,algn_val_ispw2);
	AlignedMemClip* oldClip = AlignedMemClip::GetMemClipFromAlignedPtr(ptr); //XXX
	void* const p = aligned_malloc(length_byte_0,algn_val_ispw2);
	memcpy(p,ptr,jmin(length_byte_0,oldClip->len));
	aligned_free(ptr);
	return p;
}

} //namespace treecore
