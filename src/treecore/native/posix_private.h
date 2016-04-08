#ifndef TREECORE_NATIVE_POSIX_PRIVATE_H
#define TREECORE_NATIVE_POSIX_PRIVATE_H

#include "treecore/Common.h"
#include "treecore/Config.h"
#include "treecore/MathsFunctions.h"
#include "treecore/Result.h"

#include <sys/stat.h>
#include <sys/vfs.h>

namespace treecore {

class File;
class String;
class Time;

#if TREECORE_OS_LINUX || (TREECORE_OS_IOS && !__DARWIN_ONLY_64_BIT_INO_T)  // (this iOS stuff is to avoid a simulator bug)
typedef struct stat64 _StatStruct_;
#    define TREECORE_STAT     stat64
#else
typedef struct stat _StatStruct_;
#    define TREECORE_STAT     stat
#endif

int _sig_interrupt_( int sig, int flag );

int _get_fd_( void* handle ) noexcept;

void* _fd_to_void_ptr_( int fd ) noexcept;

Result _get_errno_result_();

Result _get_return_value_result_( int value );

bool _stat_( const String& fileName, _StatStruct_& info );

bool _do_stat_fs_( File f, struct statfs& result );

File _get_executable_file_();

void _run_system_command_( const String& );

String _get_output_from_command_( const String& );

void _update_file_stat_info_( const String& path, bool* const isDir, int64* const fileSize,
                              Time* const modTime, Time* const creationTime, bool* const isReadOnly );

}

#endif // TREECORE_NATIVE_POSIX_PRIVATE_H
