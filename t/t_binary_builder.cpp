#include "treecore/TestFramework.h"

#include "treecore/ChildProcess.h"
#include "treecore/File.h"
#include "treecore/FileOutputStream.h"
#include "treecore/Logger.h"
#include "treecore/String.h"
#include "treecore/StringRef.h"
#include "treecore/Thread.h"
#include "treecore/Time.h"

#include <fstream>

using namespace treecore;

#define DIR_IN "binary_builder_test_input"
#define NAME_OUT "binary_builder_test_output"

void create_input()
{}

void TestFramework::content( int argc, char** argv )
{
    File dir_in   = File::getCurrentWorkingDirectory().getChildFile( DIR_IN );
    File file_in1 = dir_in.getChildFile( "foo" );
    File file_in2 = dir_in.getChildFile( "bar" );

    File file_out_header = File::getCurrentWorkingDirectory().getChildFile( NAME_OUT ".h" );
    File file_out_source = File::getCurrentWorkingDirectory().getChildFile( NAME_OUT ".cpp" );

    // validate arguments
    if (argc != 2)
    {
        Logger::writeToLog( "usage: " + String( argv[1] ) + " path_to_binary_builder" );
        exit( EXIT_FAILURE );
    }

    //
    // generate input
    //
    if ( dir_in.exists() ) dir_in.deleteRecursively();
    dir_in.createDirectory();

    {
        FileOutputStream fh1( file_in1 );
        fh1.writeByte( 0 );
        fh1.writeByte( 1 );
        fh1.writeByte( 2 );
        fh1.writeByte( 3 );
    }
    {
        FileOutputStream fh2( file_in2 );
        fh2.writeByte( 4 );
        fh2.writeByte( 5 );
        fh2.writeByte( 6 );
        fh2.writeByte( 7 );
    }

    OK( file_in1.exists() );
    OK( file_in2.exists() );

    const int64 mtime_in1_orig = file_in1.getLastModificationTime().toMilliseconds();
    const int64 mtime_in2_orig = file_in2.getLastModificationTime().toMilliseconds();

    Thread::sleep( 1000 );

    //
    // run first time
    //

    ChildProcess child;
    OK( child.start( String( argv[1] ) + " --in " DIR_IN " --name " NAME_OUT ) );
    OK( child.waitForProcessToFinish( -1 ) );

    // check output file
    OK( file_out_header.exists() );
    OK( file_out_source.exists() );

    const int64 ctime_header_orig = file_out_header.getLastModificationTime().toMilliseconds();
    const int64 ctime_source_orig = file_out_source.getLastModificationTime().toMilliseconds();
    LT( mtime_in1_orig, ctime_header_orig );
    LT( mtime_in2_orig, ctime_header_orig );
    LT( mtime_in1_orig, ctime_source_orig );
    LT( mtime_in2_orig, ctime_source_orig );

    Thread::sleep( 1000 );

    //
    // run in update mode
    // output shall not be modified, as they are up-to-date
    //
    OK( child.start( String( argv[1] ) + " --in " DIR_IN " --name " NAME_OUT " --update" ) );
    OK( child.waitForProcessToFinish( -1 ) );

    const int64 ctime_header_update1 = file_out_header.getLastModificationTime().toMilliseconds();
    const int64 ctime_source_update1 = file_out_source.getLastModificationTime().toMilliseconds();
    IS( ctime_header_orig, ctime_header_update1 );
    IS( ctime_source_orig, ctime_source_update1 );

    Thread::sleep( 1000 );

    //
    // modify input file and run in update mode
    //
    file_in1.setLastModificationTime( Time::getCurrentTime() );
    int64 mtime_in1_mod1 = file_in1.getLastModificationTime().toMilliseconds();
    LT( mtime_in1_orig,    mtime_in1_mod1 );
    LT( ctime_header_orig, mtime_in1_mod1 );
    LT( ctime_source_orig, mtime_in1_mod1 );

    OK( child.start( String( argv[1] ) + " --in " DIR_IN " --name " NAME_OUT " --update" ) );
    OK( child.waitForProcessToFinish( -1 ) );

    const int64 ctime_header_update2 = file_out_header.getLastModificationTime().toMilliseconds();
    const int64 ctime_source_update2 = file_out_source.getLastModificationTime().toMilliseconds();

    LT( ctime_header_orig, ctime_header_update2 );
    LT( ctime_source_orig, ctime_source_update2 );

    Thread::sleep( 1000 );

    //
    // run in override mode
    //
    OK( child.start( String( argv[1] ) + " --in " DIR_IN " --name " NAME_OUT ) );
    OK( child.waitForProcessToFinish( -1 ) );

    const int64 ctime_header_override = file_out_header.getLastModificationTime().toMilliseconds();
    const int64 ctime_source_override = file_out_source.getLastModificationTime().toMilliseconds();
    LT( ctime_header_update2, ctime_header_override );
    LT( ctime_source_update2, ctime_source_override );
}

