#ifndef TREECORE_NATIVE_WIN32_HIGH_RESOLUTION_TIMER_H
#define TREECORE_NATIVE_WIN32_HIGH_RESOLUTION_TIMER_H

#include "treecore/HighResolutionTimer.h"
#include "treecore/StandardHeader.h"
#include "treecore/BasicNativeHeaders.h"

namespace treecore
{

struct HighResolutionTimer::Pimpl
{
    Pimpl (HighResolutionTimer& t) noexcept;

    ~Pimpl();

    void start (int newPeriod);

    void stop();

    HighResolutionTimer& owner;
    int periodMs;

private:
    unsigned int timerID;

    static void __stdcall callbackFunction (UINT, UINT, DWORD_PTR userInfo, DWORD_PTR, DWORD_PTR);

    TREECORE_DECLARE_NON_COPYABLE (Pimpl)
};

} // namespace treecore

#endif // TREECORE_NATIVE_WIN32_HIGH_RESOLUTION_TIMER_H
