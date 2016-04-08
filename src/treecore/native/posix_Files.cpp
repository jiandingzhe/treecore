#include "treecore/File.h"
#include "treecore/FileInputStream.h"
#include "treecore/FileOutputStream.h"
#include "treecore/MemoryMappedFile.h"

#include "treecore/native/posix_private.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <utime.h>

namespace treecore
{

const treecore_wchar File::separator = '/';
const String File::separatorString( "/" );

File File::getCurrentWorkingDirectory()
{
    HeapBlock<char> heapBuffer;

    char localBuffer [1024];
    char* cwd = getcwd( localBuffer, sizeof(localBuffer) - 1 );
    size_t bufferSize = 4096;

    while (cwd == nullptr && errno == ERANGE)
    {
        heapBuffer.malloc( bufferSize );
        cwd = getcwd( heapBuffer, bufferSize - 1 );
        bufferSize += 1024;
    }

    return File( CharPointer_UTF8( cwd ) );
}

bool File::setAsCurrentWorkingDirectory() const
{
    return chdir( getFullPathName().toUTF8() ) == 0;
}

bool File::isDirectory() const
{
    _StatStruct_ info;

    return fullPath.isEmpty()
           || ( _stat_( fullPath, info ) && ( (info.st_mode & S_IFDIR) != 0 ) );
}

bool File::exists() const
{
    return fullPath.isNotEmpty()
           && access( fullPath.toUTF8(), F_OK ) == 0;
}

bool File::existsAsFile() const
{
    return exists() && !isDirectory();
}

int64 File::getSize() const
{
    _StatStruct_ info;
    return _stat_( fullPath, info ) ? info.st_size : 0;
}

uint64 File::getFileIdentifier() const
{
    _StatStruct_ info;
    return _stat_( fullPath, info ) ? (uint64) info.st_ino : 0;
}

bool File::hasWriteAccess() const
{
    if ( exists() )
        return access( fullPath.toUTF8(), W_OK ) == 0;

    if ( ( !isDirectory() ) && fullPath.containsChar( separator ) )
        return getParentDirectory().hasWriteAccess();

    return false;
}

bool File::setFileReadOnlyInternal( const bool shouldBeReadOnly ) const
{
    _StatStruct_ info;
    if ( !_stat_( fullPath, info ) )
        return false;

    info.st_mode &= 0777;   // Just permissions

    if (shouldBeReadOnly)
        info.st_mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
    else
        // Give everybody write permission?
        info.st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;

    return chmod( fullPath.toUTF8(), info.st_mode ) == 0;
}

void File::getFileTimesInternal( int64& modificationTime, int64& accessTime, int64& creationTime ) const
{
    modificationTime = 0;
    accessTime   = 0;
    creationTime = 0;

    _StatStruct_ info;
    if ( _stat_( fullPath, info ) )
    {
        modificationTime = (int64) info.st_mtime * 1000;
        accessTime   = (int64) info.st_atime * 1000;
        creationTime = (int64) info.st_ctime * 1000;
    }
}

bool File::setFileTimesInternal( int64 modificationTime, int64 accessTime, int64 /*creationTime*/ ) const
{
    _StatStruct_ info;

    if ( (modificationTime != 0 || accessTime != 0) && _stat_( fullPath, info ) )
    {
        struct utimbuf times;
        times.actime  = accessTime != 0       ? (time_t) (accessTime / 1000)       : info.st_atime;
        times.modtime = modificationTime != 0 ? (time_t) (modificationTime / 1000) : info.st_mtime;

        return utime( fullPath.toUTF8(), &times ) == 0;
    }

    return false;
}

bool File::deleteFile() const
{
    if ( !exists() )
        return true;

    if ( isDirectory() )
        return rmdir( fullPath.toUTF8() ) == 0;

    return remove( fullPath.toUTF8() ) == 0;
}

bool File::moveInternal( const File& dest ) const
{
    if (rename( fullPath.toUTF8(), dest.getFullPathName().toUTF8() ) == 0)
        return true;

    if ( hasWriteAccess() && copyInternal( dest ) )
    {
        if ( deleteFile() )
            return true;

        dest.deleteFile();
    }

    return false;
}

Result File::createDirectoryInternal( const String& fileName ) const
{
    return _get_return_value_result_( mkdir( fileName.toUTF8(), 0777 ) );
}

int64 File::getBytesFreeOnVolume() const
{
    struct statfs buf;
    if ( _do_stat_fs_( *this, buf ) )
        return (int64) buf.f_bsize * (int64) buf.f_bavail; // Note: this returns space available to non-super user

    return 0;
}

int64 File::getVolumeTotalSize() const
{
    struct statfs buf;
    if ( _do_stat_fs_( *this, buf ) )
        return (int64) buf.f_bsize * (int64) buf.f_blocks;

    return 0;
}

String File::getVolumeLabel() const
{
#if TREECORE_OS_OSX
    struct VolAttrBuf
    {
        u_int32_t length;
        attrreference_t mountPointRef;
        char mountPointSpace [MAXPATHLEN];
    } attrBuf;

    struct attrlist attrList;
    zerostruct( attrList ); // (can't use "= { 0 }" on this object because it's typedef'ed as a C struct)
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.volattr = ATTR_VOL_INFO | ATTR_VOL_NAME;

    File f( *this );

    for (;; )
    {
        if (getattrlist( f.getFullPathName().toUTF8(), &attrList, &attrBuf, sizeof(attrBuf), 0 ) == 0)
            return String::fromUTF8( ( (const char*) &attrBuf.mountPointRef ) + attrBuf.mountPointRef.attr_dataoffset,
                                     (int) attrBuf.mountPointRef.attr_length );

        const File parent( f.getParentDirectory() );

        if (f == parent)
            break;

        f = parent;
    }
#endif

    return String();
}

int File::getVolumeSerialNumber() const
{
    int result = 0;
/*    int fd = open (getFullPathName().toUTF8(), O_RDONLY | O_NONBLOCK);

    char info [512];

   #ifndef HDIO_GET_IDENTITY
   #define HDIO_GET_IDENTITY 0x030d
   #endif

    if (ioctl (fd, HDIO_GET_IDENTITY, info) == 0)
    {
        TREECORE_DBG (String (info + 20, 20));
        result = String (info + 20, 20).trim().getIntValue();
    }

    close (fd);*/
    return result;
}

//
// FileInputStream
//
void FileInputStream::openHandle()
{
    const int f = open( file.getFullPathName().toUTF8(), O_RDONLY, 00644 );

    if (f != -1)
        fileHandle = _fd_to_void_ptr_( f );
    else
        status = _get_errno_result_();
}

FileInputStream::~FileInputStream()
{
    if (fileHandle != 0)
        close( _get_fd_( fileHandle ) );
}

size_t FileInputStream::readInternal( void* const buffer, const size_t numBytes )
{
    ssize_t result = 0;

    if (fileHandle != 0)
    {
        result = ::read( _get_fd_( fileHandle ), buffer, numBytes );

        if (result < 0)
        {
            status = _get_errno_result_();
            result = 0;
        }
    }

    return (size_t) result;
}

//
// FileOutputStream
//
void FileOutputStream::openHandle()
{
    if ( file.exists() )
    {
        const int f = open( file.getFullPathName().toUTF8(), O_RDWR, 00644 );

        if (f != -1)
        {
            currentPosition = lseek( f, 0, SEEK_END );

            if (currentPosition >= 0)
            {
                fileHandle = _fd_to_void_ptr_( f );
            }
            else
            {
                status = _get_errno_result_();
                close( f );
            }
        }
        else
        {
            status = _get_errno_result_();
        }
    }
    else
    {
        const int f = open( file.getFullPathName().toUTF8(), O_RDWR + O_CREAT, 00644 );

        if (f != -1)
            fileHandle = _fd_to_void_ptr_( f );
        else
            status = _get_errno_result_();
    }
}

void FileOutputStream::closeHandle()
{
    if (fileHandle != 0)
    {
        close( _get_fd_( fileHandle ) );
        fileHandle = 0;
    }
}

ssize_t FileOutputStream::writeInternal( const void* const data, const size_t numBytes )
{
    ssize_t result = 0;

    if (fileHandle != 0)
    {
        result = ::write( _get_fd_( fileHandle ), data, numBytes );

        if (result == -1)
            status = _get_errno_result_();
    }

    return result;
}

void FileOutputStream::flushInternal()
{
    if (fileHandle != 0)
    {
        if (fsync( _get_fd_( fileHandle ) ) == -1)
            status = _get_errno_result_();

#if TREECORE_OS_ANDROID
        // This stuff tells the OS to asynchronously update the metadata
        // that the OS has cached aboud the file - this metadata is used
        // when the device is acting as a USB drive, and unless it's explicitly
        // refreshed, it'll get out of step with the real file.
        const LocalRef<jstring> t( javaString( file.getFullPathName() ) );
        android.activity.callVoidMethod( TreecoreAppActivity.scanFile, t.get() );
#endif
    }
}

Result FileOutputStream::truncate()
{
    if (fileHandle == 0)
        return status;

    flush();
    return _get_return_value_result_( ftruncate( _get_fd_( fileHandle ), (off_t) currentPosition ) );
}

//
// MemoryMappedFile
//

void MemoryMappedFile::openInternal( const File& file, MemoryMappedFile::AccessMode mode )
{
    treecore_assert( mode == readOnly || mode == readWrite );

    if (range.getStart() > 0)
    {
        const long pageSize = sysconf( _SC_PAGE_SIZE );
        range.setStart( range.getStart() - (range.getStart() % pageSize) );
    }

    fileHandle = open( file.getFullPathName().toUTF8(),
                       mode == readWrite ? (O_CREAT + O_RDWR) : O_RDONLY, 00644 );

    if (fileHandle != -1)
    {
        void* m = mmap( 0, (size_t) range.getLength(),
                        mode == readWrite ? (PROT_READ | PROT_WRITE) : PROT_READ,
                        MAP_SHARED, fileHandle,
                        (off_t) range.getStart() );

        if (m != MAP_FAILED)
        {
            address = m;
            madvise( m, (size_t) range.getLength(), MADV_SEQUENTIAL );
        }
        else
        {
            range = Range<int64>();
        }
    }
}

MemoryMappedFile::~MemoryMappedFile()
{
    if (address != nullptr)
        munmap( address, (size_t) range.getLength() );

    if (fileHandle != 0)
        close( fileHandle );
}

} // namespace treecore
