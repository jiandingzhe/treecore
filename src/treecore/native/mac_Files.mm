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

/*
   Note that a lot of methods that you'd expect to find in this file actually
   live in juce_posix_SharedCode.h!
 */

#include "treecore/File.h"
#include "treecore/ClassUtils.h"
#include "treecore/native/osx_ObjCHelpers.h"
#include "treecore/native/posix_private.h"
#include "treecore/DirectoryIterator.h"
#include "treecore/Process.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <fnmatch.h>
#include <mach-o/dyld.h>

namespace treecore
{

    bool doStatFS (File f, struct statfs& result)
    {
        for (int i = 5; --i >= 0;)
        {
            if (f.exists())
                break;
            
            f = f.getParentDirectory();
        }
        
        return statfs (f.getFullPathName().toUTF8(), &result) == 0;
    }
    
    //==============================================================================
bool File::copyInternal( const File& dest ) const
{
    TREECORE_AUTO_RELEASE_POOL
    {
        NSFileManager* fm = [NSFileManager defaultManager];

        return [fm fileExistsAtPath : juceStringToNS( fullPath )]
#if defined (MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
               && [fm copyItemAtPath : juceStringToNS( fullPath )
                   toPath : juceStringToNS( dest.getFullPathName() )
                   error : nil];
#else
               && [fm copyPath : juceStringToNS( fullPath )
                   toPath : juceStringToNS( dest.getFullPathName() )
                   handler : nil];
#endif
    }
}

void File::findFileSystemRoots( Array<File>& destArray )
{
    destArray.add( File( "/" ) );
}

//==============================================================================
namespace FileHelpers
{
static bool isFileOnDriveType( const File& f, const char* const* types )
{
    struct statfs buf;

    if ( doStatFS( f, buf ) )
    {
        const String type( buf.f_fstypename );

        while (*types != 0)
            if ( type.equalsIgnoreCase( *types++ ) )
                return true;
    }

    return false;
}

static bool isHiddenFile( const String& path )
{
#if defined (MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_6
    TREECORE_AUTO_RELEASE_POOL
    {
        NSNumber* hidden = nil;
        NSError* err = nil;

        return [[NSURL fileURLWithPath : juceStringToNS( path )]
                getResourceValue : &hidden forKey : NSURLIsHiddenKey error : &err]
               && [hidden boolValue];
    }
#elif TREECORE_OS_IOS
    return File( path ).getFileName().startsWithChar( '.' );
#else
    FSRef ref;
    LSItemInfoRecord info;

    return FSPathMakeRefWithOptions( (const UInt8*) path.toRawUTF8(), kFSPathMakeRefDoNotFollowLeafSymlink, &ref, 0 ) == noErr
           && LSCopyItemInfoForRef( &ref, kLSRequestBasicFlagsOnly, &info ) == noErr
           && (info.flags & kLSItemInfoIsInvisible) != 0;
#endif
}

#if TREECORE_OS_IOS
static String getIOSSystemLocation( NSSearchPathDirectory type )
{
    return nsStringToJuce([NSSearchPathForDirectoriesInDomains( type, NSUserDomainMask, YES )
                           objectAtIndex: 0] );
}
#endif

static bool launchExecutable( const String& pathAndArguments )
{
    const char* const argv[4] = { "/bin/sh", "-c", pathAndArguments.toUTF8(), 0 };

    const int cpid = fork();

    if (cpid == 0)
    {
// Child process
        if (execve( argv[0], (char**) argv, 0 ) < 0)
            exit( 0 );
    }
    else
    {
        if (cpid < 0)
            return false;
    }

    return true;
}
}

bool File::isOnCDRomDrive() const
{
    static const char* const cdTypes[] = { "cd9660", "cdfs", "cddafs", "udf", nullptr };

    return FileHelpers::isFileOnDriveType( *this, cdTypes );
}

bool File::isOnHardDisk() const
{
    static const char* const nonHDTypes[] = { "nfs", "smbfs", "ramfs", nullptr };

    return !( isOnCDRomDrive() || FileHelpers::isFileOnDriveType( *this, nonHDTypes ) );
}

bool File::isOnRemovableDrive() const
{
#if TREECORE_OS_IOS
    return false; // xxx is this possible?
#else
    TREECORE_AUTO_RELEASE_POOL
    {
        BOOL removable = false;

        [[NSWorkspace sharedWorkspace]
getFileSystemInfoForPath: juceStringToNS( getFullPathName() )
isRemovable: &removable
isWritable: nil
isUnmountable: nil
description: nil
type: nil];

        return removable;
    }
#endif
}

bool File::isHidden() const
{
    return FileHelpers::isHiddenFile( getFullPathName() );
}

//==============================================================================
const char* const* juce_argv = nullptr;
int juce_argc = 0;

File File::getSpecialLocation( const SpecialLocationType type )
{
    TREECORE_AUTO_RELEASE_POOL
    {
        String resultPath;

        switch (type)
        {
        case userHomeDirectory:                 resultPath = nsStringToJuce( NSHomeDirectory() ); break;

#if TREECORE_OS_IOS
        case userDocumentsDirectory:            resultPath = FileHelpers::getIOSSystemLocation( NSDocumentDirectory ); break;
        case userDesktopDirectory:              resultPath = FileHelpers::getIOSSystemLocation( NSDesktopDirectory ); break;

        case tempDirectory:
        {
            File tmp( FileHelpers::getIOSSystemLocation( NSCachesDirectory ) );
            tmp = tmp.getChildFile( _get_executable_file_().getFileNameWithoutExtension() );
            tmp.createDirectory();
            return tmp.getFullPathName();
        }

#else
        case userDocumentsDirectory:            resultPath = "~/Documents"; break;
        case userDesktopDirectory:              resultPath = "~/Desktop"; break;

        case tempDirectory:
        {
            File tmp( "~/Library/Caches/" + _get_executable_file_().getFileNameWithoutExtension() );
            tmp.createDirectory();
            return File( tmp.getFullPathName() );
        }
#endif
        case userMusicDirectory:                resultPath = "~/Music"; break;
        case userMoviesDirectory:               resultPath = "~/Movies"; break;
        case userPicturesDirectory:             resultPath = "~/Pictures"; break;
        case userApplicationDataDirectory:      resultPath = "~/Library"; break;
        case commonApplicationDataDirectory:    resultPath = "/Library"; break;
        case commonDocumentsDirectory:          resultPath = "/Users/Shared"; break;
        case globalApplicationsDirectory:       resultPath = "/Applications"; break;

        case invokedExecutableFile:
            if (juce_argv != nullptr && juce_argc > 0)
                return File( CharPointer_UTF8( juce_argv[0] ) );
// deliberate fall-through...

        case currentExecutableFile:
            return _get_executable_file_();

        case currentApplicationFile:
        {
            const File exe( _get_executable_file_() );
            const File parent( exe.getParentDirectory() );

#if TREECORE_OS_IOS
            return parent;
#else
            return parent.getFullPathName().endsWithIgnoreCase( "Contents/MacOS" )
                   ? parent.getParentDirectory().getParentDirectory()
                   : exe;
#endif
        }

        case hostApplicationPath:
        {
            unsigned int size = 8192;
            HeapBlock<char> buffer;
            buffer.calloc( size + 8 );

            _NSGetExecutablePath( buffer.getData(), &size );
            return File( String::fromUTF8( buffer, (int) size ) );
        }

        default:
            treecore_assert_false; // unknown type?
            break;
        }

        if ( resultPath.isNotEmpty() )
            return File( resultPath.convertToPrecomposedUnicode() );
    }

    return File();
}

//==============================================================================
String File::getVersion() const
{
    TREECORE_AUTO_RELEASE_POOL
    {
        if (NSBundle* bundle = [NSBundle bundleWithPath: juceStringToNS( getFullPathName() )])
            if (NSDictionary* info = [bundle infoDictionary])
                if (NSString* name = [info valueForKey: nsStringLiteral( "CFBundleShortVersionString" )])
                    return nsStringToJuce( name );
    }

    return String();
}

//==============================================================================
static NSString* getFileLink( const String& path )
{
#if TREECORE_OS_IOS || (defined (MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
    return [[NSFileManager defaultManager] destinationOfSymbolicLinkAtPath : juceStringToNS( path ) error : nil];
#else
// (the cast here avoids a deprecation warning)
    return [( (id) [NSFileManager defaultManager] )pathContentOfSymbolicLinkAtPath : juceStringToNS( path )];
#endif
}

bool File::isLink() const
{
    return getFileLink( fullPath ) != nil;
}

File File::getLinkedTarget() const
{
    if ( NSString* dest = getFileLink( fullPath ) )
        return getSiblingFile( nsStringToJuce( dest ) );

    return *this;
}

//==============================================================================
bool File::moveToTrash() const
{
    if ( !exists() )
        return true;

#if TREECORE_OS_IOS
    return deleteFile(); //xxx is there a trashcan on the iOS?
#else
    TREECORE_AUTO_RELEASE_POOL
    {
        NSString* p = juceStringToNS( getFullPathName() );

        return [[NSWorkspace sharedWorkspace]
performFileOperation: NSWorkspaceRecycleOperation
source: [p stringByDeletingLastPathComponent]
destination: nsEmptyString()
files: [NSArray arrayWithObject : [p lastPathComponent]]
                tag : nil];
    }
#endif
}

//==============================================================================
class DirectoryIterator::NativeIterator::Pimpl
{
public:
    Pimpl ( const File& directory, const String& wildCard_ )
        : parentDir( File::addTrailingSeparator( directory.getFullPathName() ) ),
        wildCard( wildCard_ ),
        enumerator( nil )
    {
        TREECORE_AUTO_RELEASE_POOL
        {
            enumerator = [[[NSFileManager defaultManager] enumeratorAtPath: juceStringToNS( directory.getFullPathName() )] retain];
        }
    }

    ~Pimpl()
    {
        [enumerator release];
    }

    bool next( String& filenameFound,
               bool* const isDir, bool* const isHidden, int64* const fileSize,
               Time* const modTime, Time* const creationTime, bool* const isReadOnly )
    {
        TREECORE_AUTO_RELEASE_POOL
        {
            const char* wildcardUTF8 = nullptr;

            for (;; )
            {
                NSString* file;
                if (enumerator == nil || (file = [enumerator nextObject]) == nil)
                    return false;

                [enumerator skipDescendents];
                filenameFound = nsStringToJuce( file ).convertToPrecomposedUnicode();

                if (wildcardUTF8 == nullptr)
                    wildcardUTF8 = wildCard.toUTF8();

                if (fnmatch( wildcardUTF8, filenameFound.toUTF8(), FNM_CASEFOLD ) != 0)
                    continue;

                const String fullPath( parentDir + filenameFound );
                _update_file_stat_info_( fullPath, isDir, fileSize, modTime, creationTime, isReadOnly );

                if (isHidden != nullptr)
                    *isHidden = FileHelpers::isHiddenFile( fullPath );

                return true;
            }
        }
    }

private:
    String parentDir, wildCard;
    NSDirectoryEnumerator* enumerator;

    TREECORE_DECLARE_NON_COPYABLE( Pimpl )
};

DirectoryIterator::NativeIterator::NativeIterator ( const File& directory, const String& wildcard )
    : pimpl( new DirectoryIterator::NativeIterator::Pimpl( directory, wildcard ) )
{}

DirectoryIterator::NativeIterator::~NativeIterator()
{}

bool DirectoryIterator::NativeIterator::next( String& filenameFound,
                                              bool* const isDir, bool* const isHidden, int64* const fileSize,
                                              Time* const modTime, Time* const creationTime, bool* const isReadOnly )
{
    return pimpl->next( filenameFound, isDir, isHidden, fileSize, modTime, creationTime, isReadOnly );
}

//==============================================================================
bool TREECORE_STDCALL Process::openDocument( const String& fileName, const String& parameters )
{
    TREECORE_AUTO_RELEASE_POOL
    {
        NSURL* filenameAsURL = [NSURL URLWithString: juceStringToNS( fileName )];

#if TREECORE_OS_IOS
        (void) parameters;
        return [[UIApplication sharedApplication] openURL : filenameAsURL];
#else
        NSWorkspace* workspace = [NSWorkspace sharedWorkspace];

        if ( parameters.isEmpty() )
            return [workspace openFile : juceStringToNS( fileName )]
                   || [workspace openURL : filenameAsURL];

        const File file( fileName );

        if ( file.isBundle() )
        {
            StringArray params;
            params.addTokens( parameters, true );

            NSMutableArray* paramArray = [[[NSMutableArray alloc] init] autorelease];
            for (int i = 0; i < params.size(); ++i)
                [paramArray addObject : juceStringToNS( params[i] )];

            NSMutableDictionary* dict = [[[NSMutableDictionary alloc] init] autorelease];
            [dict setObject : paramArray
             forKey : nsStringLiteral( "NSWorkspaceLaunchConfigurationArguments" )];

            return [workspace launchApplicationAtURL : filenameAsURL
                    options : NSWorkspaceLaunchDefault | NSWorkspaceLaunchNewInstance
                    configuration : dict
                    error : nil];
        }

        if ( file.exists() )
            return FileHelpers::launchExecutable( "\"" + fileName + "\" " + parameters );

        return false;
#endif
    }
}

void File::revealToUser() const
{
#if !TREECORE_OS_IOS
    if ( exists() )
        [[NSWorkspace sharedWorkspace] selectFile : juceStringToNS( getFullPathName() ) inFileViewerRootedAtPath : nsEmptyString()];
    else if ( getParentDirectory().exists() )
        getParentDirectory().revealToUser();
#endif
}

//==============================================================================
OSType File::getMacOSType() const
{
    TREECORE_AUTO_RELEASE_POOL
    {
#if TREECORE_OS_IOS || (defined (MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
        NSDictionary* fileDict = [[NSFileManager defaultManager] attributesOfItemAtPath: juceStringToNS( getFullPathName() ) error: nil];
#else
// (the cast here avoids a deprecation warning)
        NSDictionary* fileDict = [( (id) [NSFileManager defaultManager] )fileAttributesAtPath: juceStringToNS( getFullPathName() ) traverseLink: NO];
#endif

        return [fileDict fileHFSTypeCode];
    }
}

bool File::isBundle() const
{
#if TREECORE_OS_IOS
    return false; // xxx can't find a sensible way to do this without trying to open the bundle..
#else
    TREECORE_AUTO_RELEASE_POOL
    {
        return [[NSWorkspace sharedWorkspace] isFilePackageAtPath : juceStringToNS( getFullPathName() )];
    }
#endif
}

#if TREECORE_OS_OSX
void File::addToDock() const
{
// check that it's not already there...
    if ( !_get_output_from_command_( "defaults read com.apple.dock persistent-apps" ).containsIgnoreCase( getFullPathName() ) )
    {
        _run_system_command_( "defaults write com.apple.dock persistent-apps -array-add \"<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>"
                               + getFullPathName() + "</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>\"" );

        _run_system_command_( "osascript -e \"tell application \\\"Dock\\\" to quit\"" );
    }
}
#endif

} // namespace treecore
