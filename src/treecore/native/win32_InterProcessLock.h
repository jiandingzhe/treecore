#ifndef TREECORE_NATIVE_WIN32_INTER_PROCESS_LOCK_H
#define TREECORE_NATIVE_WIN32_INTER_PROCESS_LOCK_H

#include "treecore/InterProcessLock.h"
#include <Windows.h>

namespace treecore
{

class InterProcessLock::Pimpl
{
public:
    Pimpl (String name, const int timeOutMillisecs);
    ~Pimpl();
    void close();

    HANDLE handle;
    int refCount;
};

} // namespace treecore

#endif // TREECORE_NATIVE_WIN32_INTER_PROCESS_LOCK_H
