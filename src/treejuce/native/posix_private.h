#ifndef TREEJUCE_NATIVE_POSIX_PRIVATE_H
#define TREEJUCE_NATIVE_POSIX_PRIVATE_H

#include "treejuce/Common.h"
#include "treejuce/Config.h"
#include "treejuce/MathsFunctions.h"

namespace treejuce {

class File;
class String;
class Time;

#if defined TREEJUCE_OS_LINUX || (defined TREEJUCE_OS_IOS && ! __DARWIN_ONLY_64_BIT_INO_T) // (this iOS stuff is to avoid a simulator bug)
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

#endif // TREEJUCE_NATIVE_POSIX_PRIVATE_H
