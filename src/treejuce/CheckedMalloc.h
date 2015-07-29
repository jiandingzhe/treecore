#ifndef ztd_aligffffffffffffffffffffned_malloc_h__
#define ztd_aligffffffffffffffffffffned_malloc_h__

#include "treejuce/IntTypes.h"
#include "treejuce/PlatformDefs.h"

namespace treejuce
{

void* checked_malloc(size_t len);

void* checked_calloc(size_t len);

void checked_free(void* ptr);

void* checked_realloc(void* const ptr,size_t len);

} // namespace treejuce

#endif // ztd_aligned_malloc_h__
