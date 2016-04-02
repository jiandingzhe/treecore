#ifndef TREECORE_NATIVE_POSIX_PRIVATE_H
#define TREECORE_NATIVE_POSIX_PRIVATE_H

#include "treecore/Common.h"
#include "treecore/Config.h"
#include "treecore/MathsFunctions.h"

namespace treecore {

class File;
class String;
class Time;

#if defined TREECORE_OS_LINUX || (defined TREECORE_OS_IOS && ! __DARWIN_ONLY_64_BIT_INO_T) // (this iOS stuff is to avoid a simulator bug)
typedef struct stat64 juce_statStruct;
#define JUCE_STAT     stat64
#else
typedef struct stat   juce_statStruct;
#define JUCE_STAT     stat
#endif

bool juce_stat(const String& fileName, juce_statStruct& info);

File juce_getExecutableFile();

void updateStatInfoForFile(const String& path, bool* const isDir, int64* const fileSize,
                           Time* const modTime, Time* const creationTime, bool* const isReadOnly);

}

#endif // TREECORE_NATIVE_POSIX_PRIVATE_H
