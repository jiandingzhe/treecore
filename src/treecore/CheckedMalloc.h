#ifndef TREECORE_CHECKED_MALLOC_H
#define TREECORE_CHECKED_MALLOC_H

#include "treecore/IntTypes.h"

namespace treecore
{

void* checked_malloc(size_t len);

void* checked_calloc(size_t len);

void checked_free(void* ptr);

void* checked_realloc(void* const ptr, size_t len);

} // namespace treecore

#endif // TREECORE_CHECKED_MALLOC_H
