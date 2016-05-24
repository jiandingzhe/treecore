#include "treecore/TestFramework.h"
#include "TestConfig.h"

#include "treecore/File.h"
#include "treecore/FileInputStream.h"
#include "treecore/StringRef.h"

using namespace treecore;

#define OUTPUT_NAME "build_time_resource_wrap_output"

int item_size( const File& file, const String& name )
{
    FileInputStream fh( file );

    while ( !fh.isExhausted() )
    {
        String line = fh.readNextLine();
        int i = line.indexOf( name + "Size" );

        if (i >= 0)
        {
            String value = line.substring( i + name.length() + 4 );
            value = value.removeCharacters( " =;" );
            return value.getIntValue();
        }
    }
    return -1;
}

void TestFramework::content( int, char** )
{
    File file_out_header = File( TREECORE_BINARY_DIR ).getChildFile( "t" ).getChildFile( OUTPUT_NAME ".h" );
    File file_out_source_foo = File( TREECORE_BINARY_DIR ).getChildFile( "t" ).getChildFile( "foo.cpp" );
    File file_out_source_bar = File( TREECORE_BINARY_DIR ).getChildFile( "t" ).getChildFile( "bar.cpp" );
    File file_out_source_baz = File( TREECORE_BINARY_DIR ).getChildFile( "t" ).getChildFile( "baz.cpp" );
    OK( file_out_header.existsAsFile() );
    OK( file_out_source_foo.existsAsFile() );
    OK( file_out_source_bar.existsAsFile() );
    OK( file_out_source_baz.existsAsFile() );

    IS( item_size( file_out_header, "foo" ),  9 );
    IS( item_size( file_out_header, "bar" ),  9 );
    IS( item_size( file_out_header, "baz" ), 10 );
}
