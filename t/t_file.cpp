#include "treecore/File.h"
#include "treecore/FileOutputStream.h"
#include "treecore/MemoryBlock.h"
#include "treecore/MemoryMappedFile.h"
#include "treecore/StringRef.h"
#include "treecore/Time.h"

#include "TestFramework.h"

using namespace treecore;

void TestFramework::content()
{
    OK( "Reading" );

    const File home( File::getSpecialLocation( File::userHomeDirectory ) );
    const File temp( File::getSpecialLocation( File::tempDirectory ) );

    OK( !File::nonexistent.exists() );
    OK( home.isDirectory() );
    OK( home.exists() );
    OK( !home.existsAsFile() );
    OK( File::getSpecialLocation( File::userDocumentsDirectory ).isDirectory() );
    OK( File::getSpecialLocation( File::userApplicationDataDirectory ).isDirectory() );
    OK( File::getSpecialLocation( File::currentExecutableFile ).exists() );
    OK( File::getSpecialLocation( File::currentApplicationFile ).exists() );
    OK( File::getSpecialLocation( File::invokedExecutableFile ).exists() );
    GT( home.getVolumeTotalSize(), 1024 * 1024 );
    GT( home.getBytesFreeOnVolume(),  0 );
    OK( !home.isHidden() );
    OK( home.isOnHardDisk() );
    OK( !home.isOnCDRomDrive() );
    OK( File::getCurrentWorkingDirectory().exists() );
    OK( home.setAsCurrentWorkingDirectory() );
    IS( File::getCurrentWorkingDirectory(), home );

    {
        Array<File> roots;
        File::findFileSystemRoots( roots );
        GT( roots.size(), 0 );

        int numRootsExisting = 0;
        for (int i = 0; i < roots.size(); ++i)
            if ( roots[i].exists() )
                ++numRootsExisting;

        // (on windows, some of the drives may not contain media, so as long as at least one is ok..)
        GT( numRootsExisting, 0 );
    }

    OK( "Writing" );

    File demoFolder( temp.getChildFile( "Juce UnitTests Temp Folder.folder" ) );
    OK( demoFolder.deleteRecursively() );
    OK( demoFolder.createDirectory() );
    OK( demoFolder.isDirectory() );
    IS( demoFolder.getParentDirectory(), temp );
    OK( temp.isDirectory() );

    {
        Array<File> files;
        temp.findChildFiles( files, File::findFilesAndDirectories, false, "*" );
        OK( files.contains( demoFolder ) );
    }

    {
        Array<File> files;
        temp.findChildFiles( files, File::findDirectories, true, "*.folder" );
        OK( files.contains( demoFolder ) );
    }

    File tempFile( demoFolder.getNonexistentChildFile( "test", ".txt", false ) );

    IS( tempFile.getFileExtension(), ".txt" );
    OK( tempFile.hasFileExtension( ".txt" ) );
    OK( tempFile.hasFileExtension( "txt" ) );
    OK( tempFile.withFileExtension( "xyz" ).hasFileExtension( ".xyz" ) );
    OK( tempFile.withFileExtension( "xyz" ).hasFileExtension( "abc;xyz;foo" ) );
    OK( tempFile.withFileExtension( "xyz" ).hasFileExtension( "xyz;foo" ) );
    OK( !tempFile.withFileExtension( "h" ).hasFileExtension( "bar;foo;xx" ) );
    OK( tempFile.getSiblingFile( "foo" ).isAChildOf( temp ) );
    OK( tempFile.hasWriteAccess() );

    {
        FileOutputStream fo( tempFile );
        fo.write( "0123456789", 10 );
    }

    OK( tempFile.exists() );
    IS( tempFile.getSize(), 10 );
    LT( std::abs( (int) ( tempFile.getLastModificationTime().toMilliseconds() - Time::getCurrentTime().toMilliseconds() ) ), 3000 );
    IS( tempFile.loadFileAsString(), String( "0123456789" ) );
    OK( !demoFolder.containsSubDirectories() );

    IS( tempFile.getRelativePathFrom( demoFolder.getParentDirectory() ), demoFolder.getFileName() + File::separatorString + tempFile.getFileName() );
    IS( demoFolder.getParentDirectory().getRelativePathFrom( tempFile ),       ".." + File::separatorString + ".." + File::separatorString + demoFolder.getParentDirectory().getFileName() );

    IS( demoFolder.getNumberOfChildFiles( File::findFiles ),                      1 );
    IS( demoFolder.getNumberOfChildFiles( File::findFilesAndDirectories ),        1 );
    IS( demoFolder.getNumberOfChildFiles( File::findDirectories ),                0 );

    demoFolder.getNonexistentChildFile( "tempFolder", "", false ).createDirectory();
    IS( demoFolder.getNumberOfChildFiles( File::findDirectories ),         1 );
    IS( demoFolder.getNumberOfChildFiles( File::findFilesAndDirectories ), 2 );
    OK( demoFolder.containsSubDirectories() );

    OK( tempFile.hasWriteAccess() );
    tempFile.setReadOnly( true );
    OK( !tempFile.hasWriteAccess() );
    tempFile.setReadOnly( false );
    OK( tempFile.hasWriteAccess() );

    Time t( Time::getCurrentTime() );
    tempFile.setLastModificationTime( t );
    Time t2 = tempFile.getLastModificationTime();
    LE( std::abs( (int) ( t2.toMilliseconds() - t.toMilliseconds() ) ), 1000 );

    {
        MemoryBlock mb;
        tempFile.loadFileAsData( mb );
        IS( mb.getSize(), 10 );
        IS( mb[0],       '0' );
    }

    {
        IS( tempFile.getSize(), 10 );
        FileOutputStream fo( tempFile );
        OK( fo.openedOk() );

        OK( fo.setPosition( 7 ) );
        OK( fo.truncate().wasOk() );
        IS( tempFile.getSize(), 7 );
        fo.write( "789", 3 );
        fo.flush();
        IS( tempFile.getSize(), 10 );
    }

    OK( "Memory-mapped files" );

    {
        MemoryMappedFile mmf( tempFile, MemoryMappedFile::readOnly );
        IS( mmf.getSize(), 10 );
        OK( mmf.getData() != nullptr );
        IS( memcmp( mmf.getData(), "0123456789", 10 ), 0 );
    }

    {
        const File tempFile2( tempFile.getNonexistentSibling( false ) );
        OK( tempFile2.create() );
        OK( tempFile2.appendData( "xxxxxxxxxx", 10 ) );

        {
            MemoryMappedFile mmf( tempFile2, MemoryMappedFile::readWrite );
            IS( mmf.getSize(), 10 );
            OK( mmf.getData() != nullptr );
            memcpy( mmf.getData(), "abcdefghij", 10 );
        }

        {
            MemoryMappedFile mmf( tempFile2, MemoryMappedFile::readWrite );
            IS( mmf.getSize(), 10 );
            OK( mmf.getData() != nullptr );
            IS( memcmp( mmf.getData(), "abcdefghij", 10 ), 0 );
        }

        OK( tempFile2.deleteFile() );
    }

    OK( "More writing" );

    OK( tempFile.appendData( "abcdefghij", 10 ) );
    IS( tempFile.getSize(), 20 );
    OK( tempFile.replaceWithData( "abcdefghij", 10 ) );
    IS( tempFile.getSize(), 10 );

    File tempFile2( tempFile.getNonexistentSibling( false ) );
    OK( tempFile.copyFileTo( tempFile2 ) );
    OK( tempFile2.exists() );
    OK( tempFile2.hasIdenticalContentTo( tempFile ) );
    OK( tempFile.deleteFile() );
    OK( !tempFile.exists() );
    OK( tempFile2.moveFileTo( tempFile ) );
    OK( tempFile.exists() );
    OK( !tempFile2.exists() );

    OK( demoFolder.deleteRecursively() );
    OK( !demoFolder.exists() );
}
