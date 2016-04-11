#include "treecore/DebugUtils.h"

#include "treecore/Process.h"

namespace treecore
{

TREECORE_SHARED_API bool TREECORE_STDCALL isRunningUnderDebugger()
{
    return Process::isRunningUnderDebugger();
}

} // namespace treecore
