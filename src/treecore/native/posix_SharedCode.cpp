/*
   ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

   ==============================================================================
 */

#include "treecore/Common.h"

#include "treecore/ChildProcess.h"
#include "treecore/DynamicLibrary.h"
#include "treecore/File.h"
#include "treecore/FileInputStream.h"
#include "treecore/FileOutputStream.h"
#include "treecore/InterProcessLock.h"
#include "treecore/MemoryMappedFile.h"
#include "treecore/MT19937.h"
#include "treecore/Process.h"
#include "treecore/String.h"
#include "treecore/StringArray.h"
#include "treecore/StringRef.h"
#include "treecore/SystemStats.h"
#include "treecore/Thread.h"
#include "treecore/Time.h"

#include "treecore/native/posix_private.h"
#include "treecore/native/posix_ChildProcess.h"
#include "treecore/native/posix_InterProcessLock.h"

#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

namespace treecore {

int _sig_interrupt_( int sig, int flag )
{
    struct ::sigaction act;
    (void) ::sigaction( sig, nullptr, &act );

    if (flag != 0)
        act.sa_flags &= ~SA_RESTART;
    else
        act.sa_flags |= SA_RESTART;

    return ::sigaction( sig, &act, nullptr );
}

bool _stat_( const String& fileName, _StatStruct_& info )
{
    return fileName.isNotEmpty()
           && TREECORE_STAT( fileName.toUTF8(), &info ) == 0;
}

// if this file doesn't exist, find a parent of it that does..
bool _do_stat_fs_( File f, struct statfs& result )
{
    for (int i = 5; --i >= 0; )
    {
        if ( f.exists() )
            break;

        f = f.getParentDirectory();
    }

    return statfs( f.getFullPathName().toUTF8(), &result ) == 0;
}

void _update_file_stat_info_( const String& path, bool* const isDir, int64* const fileSize,
                            Time* const modTime, Time* const creationTime, bool* const isReadOnly )
{
    if (isDir != nullptr || fileSize != nullptr || modTime != nullptr || creationTime != nullptr)
    {
        _StatStruct_ info;
        const bool statOk = _stat_( path, info );

        if (isDir != nullptr)         *isDir = statOk && ( (info.st_mode & S_IFDIR) != 0 );
        if (fileSize != nullptr)      *fileSize = statOk ? info.st_size : 0;
        if (modTime != nullptr)       *modTime = Time( statOk ? int64( info.st_mtime * 1000 ) : 0 );
        if (creationTime != nullptr)  *creationTime = Time( statOk ? int64( info.st_ctime * 1000 ) : 0 );
    }

    if (isReadOnly != nullptr)
        *isReadOnly = access( path.toUTF8(), W_OK ) != 0;
}

Result _get_errno_result_()
{
    return Result::fail( String( strerror( errno ) ) );
}

Result _get_return_value_result_( int value )
{
    return value == -1 ? _get_errno_result_() : Result::ok();
}

int _get_fd_( void* handle ) noexcept        { return (int) (pointer_sized_int) handle; }
void* _fd_to_void_ptr_( int fd ) noexcept  { return (void*) (pointer_sized_int) fd; }

//==============================================================================

//=====================================================================
int64 treecore_fileSetPosition( void* handle, int64 pos )
{
    if (handle != 0 && lseek( _get_fd_( handle ), pos, SEEK_SET ) == pos)
        return pos;

    return -1;
}

//==============================================================================

//==============================================================================
String SystemStats::getEnvironmentVariable( const String& name, const String& defaultValue )
{
    if ( const char* s = ::getenv( name.toUTF8() ) )
        return String::fromUTF8( s );

    return defaultValue;
}

//==============================================================================

//==============================================================================

File _get_executable_file_()
{
#if TREECORE_OS_ANDROID
    return File( android.appFile );
#else
    struct DLAddrReader
    {
        static String getFilename()
        {
            Dl_info exeInfo;
            dladdr( (void*) _get_executable_file_, &exeInfo );
            return CharPointer_UTF8( exeInfo.dli_fname );
        }
    };

    static String filename( DLAddrReader::getFilename() );
    return File::getCurrentWorkingDirectory().getChildFile( filename );
#endif
}

void _run_system_command_( const String& command )
{
    int result = system( command.toUTF8() );
    (void) result;
}

String _get_output_from_command_( const String& command )
{
    // slight bodge here, as we just pipe the output into a temp file and read it...
    const File tempFile( File::getSpecialLocation( File::tempDirectory )
                         .getNonexistentChildFile( String::toHexString( int( MT19937::getInstance()->next_uint64() ) ), ".tmp", false ) );

    _run_system_command_( command + " > " + tempFile.getFullPathName() );

    String result( tempFile.loadFileAsString() );
    tempFile.deleteFile();
    return result;
}

bool DynamicLibrary::open( const String& name )
{
    close();
    handle = dlopen( name.isEmpty() ? nullptr : name.toUTF8().getAddress(), RTLD_LOCAL | RTLD_NOW );
    return handle != nullptr;
}

void DynamicLibrary::close()
{
    if (handle != nullptr)
    {
        dlclose( handle );
        handle = nullptr;
    }
}

void* DynamicLibrary::getFunction( const String& functionName ) noexcept
{
    return handle != nullptr ? dlsym( handle, functionName.toUTF8() ) : nullptr;
}

}
