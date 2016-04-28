#include "treecore/TestFramework.h"
#include "treecore/TextDiff.h"
#include "treecore/MT19937.h"

using namespace treecore;

String createString()
{
    treecore_wchar buffer[50] = { 0 };

    MT19937* rng = MT19937::getInstance();

    for (int i = int( rng->next_uint64_in_range( 49 ) ); --i >= 0; )
    {
        if (rng->next_uint64_in_range( 10 ) == 0)
        {
            do
            {
                buffer[i] = treecore_wchar( 1 + rng->next_uint64_in_range( 0x10ffff - 1 ) );
            }
            while ( !CharPointer_UTF16::canRepresent( buffer[i] ) );
        }
        else
            buffer[i] = treecore_wchar( 'a' + rng->next_uint64_in_range( 3 ) );
    }

    return CharPointer_UTF32( buffer );
}

bool test_diff( const String& a, const String& b )
{
    TextDiff diff( a, b );
    const String result( diff.appliedTo( a ) );
    return result == b;
}

void TestFramework::content( int argc, char** argv )
{
    OK( test_diff( String::empty(), String::empty() ) );
    OK( test_diff( "x",             String::empty() ) );
    OK( test_diff( String::empty(), "x" ) );
    OK( test_diff( "x",             "x" ) );
    OK( test_diff( "x",             "y" ) );
    OK( test_diff( "xxx",           "x" ) );
    OK( test_diff( "x",             "xxx" ) );

    for (int i = 5000; --i >= 0; )
    {
        String s( createString() );
        OK( test_diff( s,                     createString() ) );
        OK( test_diff( s + createString(), s + createString() ) );
    }

    MT19937::releaseInstance();
}
