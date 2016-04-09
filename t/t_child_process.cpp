#include "treecore/ChildProcess.h"

#include "TestFramework.h"

void TestFramework::content()
{
#if TREECORE_OS_WINDOWS || TREECORE_OS_OSX || TREECORE_OS_LINUX || TREECORE_OS_FREEBSD
    treecore::ChildProcess p;

#    if TREECORE_OS_WINDOWS
    OK( p.start( "tasklist" ) );
#    else
    OK( p.start( "ls /" ) );
#    endif

#endif
}

