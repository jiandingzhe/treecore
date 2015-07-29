#include "treejuce/CheckedMalloc.h"

#include <malloc.h>
#include <new>

namespace treejuce {

#if JUCE_DEBUG

void* checked_malloc(size_t len)
{
	unlikely_if(len==0) return nullptr;

	const size_t realLen = len+sizeof(size_t)+sizeof(int64)*2;

	char* ptr = (char*)malloc(realLen);
	unlikely_if( ptr == nullptr ) throw std::bad_alloc();

	int64* magicHeadPtr = (int64*)( ptr );
	*magicHeadPtr = 0xD221A6BE96E04673UL;
	ptr += sizeof( int64 );

	size_t* lenPtr = (size_t*)ptr;
	*lenPtr = len;
	ptr += sizeof( size_t );

	int64* endPtr = (int64*)( ptr +len );
	*endPtr = 0xD221A6BE96E04673UL;

	return ptr;
}

void check_memory(void* ptr)
{
	size_t* lenPtr = (size_t*)( (char*)ptr - sizeof( size_t ) );
	const size_t lenByte = *lenPtr;

	int64* endPtr = (int64*)( (char*)ptr + lenByte );
	jassert(*endPtr == 0xD221A6BE96E04673UL);

	int64* magicHeadPtr = (int64*)( (char*)ptr - sizeof(size_t)-sizeof( int64 ) );
	jassert(*magicHeadPtr == 0xD221A6BE96E04673UL);
}

void* checked_calloc(size_t len)
{
	unlikely_if(len==0) return nullptr;
	void* ptr = checked_malloc(len);
	check_memory(ptr);
	zeromem(ptr,len);
	return ptr;
}

void checked_free(void* ptr)
{
	unlikely_if(ptr == nullptr) return;
	check_memory(ptr);
	free( (char*)ptr - sizeof( size_t ) - sizeof( int64 ) );
}

void* checked_realloc(void* const ptr,size_t len)
{
	unlikely_if(ptr==nullptr) return checked_malloc(len);
	
	check_memory(ptr);
	
	size_t* lenPtr = (size_t*)( (char*)ptr - sizeof( size_t ) );
	const size_t lenByte = *lenPtr;
	if(lenByte==len) return ptr;

	char* newPtr = (char*)realloc( (char*)ptr-sizeof(size_t)-sizeof(int64),len+sizeof(int64)+sizeof(int64)*2 );
	unlikely_if( newPtr == nullptr ) throw std::bad_alloc();

	int64* magicHeadPtr = (int64*)( newPtr );
	jassert( *magicHeadPtr == 0xD221A6BE96E04673UL);
	newPtr += sizeof( int64 );

	size_t* newLenPtr = (size_t*)newPtr;
	*newLenPtr = len;
	newPtr += sizeof( size_t );

	int64* endPtr = (int64*)( newPtr + len );
	*endPtr = 0xD221A6BE96E04673UL;

	check_memory(newPtr);
	return newPtr;
}

#else

void* checked_malloc(size_t len)
{
	unlikely_if(len==0) return nullptr;
	void* p= malloc(len);
	unlikely_if(p == nullptr) throw std::bad_alloc();
	return p;
}

void* checked_calloc(size_t len)
{
	unlikely_if(len == 0) return nullptr;
	void* p= calloc(len,1);
	unlikely_if(p == nullptr) throw std::bad_alloc();
	return p;
}

void checked_free(void* ptr)
{
	unlikely_if(ptr == nullptr) return;
	free(ptr);
}

void* checked_realloc(void* const ptr,size_t len)
{
	unlikely_if(ptr==nullptr) return checked_malloc(len);
	void* p= realloc(ptr,len);
	unlikely_if(p == nullptr) throw std::bad_alloc();
	return p;
}



#endif // JUCE_DEBUG

} // namespace treejuce
