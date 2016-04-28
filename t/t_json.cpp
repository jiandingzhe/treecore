#include "treecore/DynamicObject.h"
#include "treecore/JSON.h"
#include "treecore/MT19937.h"
#include "treecore/StringPool.h"
#include "treecore/Variant.h"

#include "treecore/TestFramework.h"

using namespace treecore;

String createRandomWideCharString()
{
    MT19937& r = *MT19937::getInstance();
    treecore_wchar buffer[20] = { 0 };

    for (int i = 0; i < numElementsInArray( buffer ) - 1; ++i)
    {
        if ( r.next_bool() )
        {
            do
            {
                buffer[i] = treecore_wchar( 1 + r.next_uint64_in_range( 0x10ffff - 1 ) );
            }
            while ( !CharPointer_UTF16::canRepresent( buffer[i] ) );
        }
        else
            buffer[i] = treecore_wchar( 1 + r.next_uint64_in_range( 0xff ) );
    }

    return CharPointer_UTF32( buffer );
}

String createRandomIdentifier()
{
    MT19937& r = *MT19937::getInstance();
    char buffer[10] = { 0 };

    for (int i = 0; i < numElementsInArray( buffer ) - 1; ++i)
    {
        static const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-:";
        buffer[i] = chars [r.next_uint64_in_range( sizeof(chars) - 1 )];
    }

    return CharPointer_ASCII( buffer );
}

var createRandomVar( int depth )
{
    MT19937& r = *MT19937::getInstance();

    switch ( r.next_uint64_in_range( depth > 3 ? 6 : 8 ) )
    {
    case 0:     return var();
    case 1:     return int( r.next_int63() );
    case 2:     return r.next_int63();
    case 3:     return r.next_bool();
    case 4:     return String( r.next_double_yn(), 20 ).getDoubleValue();
    case 5:     return createRandomWideCharString();

    case 6:
    {
        var v( createRandomVar( depth + 1 ) );

        for (int i = int( 1 + r.next_uint64_in_range( 20 ) ); --i >= 0; )
            v.append( createRandomVar( depth + 1 ) );

        return v;
    }

    case 7:
    {
        DynamicObject* o = new DynamicObject();

        for (int i = int( r.next_uint64_in_range( 20 ) ); --i >= 0; )
            o->setProperty( createRandomIdentifier(), createRandomVar( depth + 1 ) );

        return o;
    }

    default:
        return var();
    }
}

void TestFramework::content( int argc, char** argv )
{
    MT19937& r = *MT19937::getInstance();

    OK( "JSON" );

    IS( JSON::parse( String::empty() ), var::null );
    OK( JSON::parse( "{}" ).isObject() );
    OK( JSON::parse( "[]" ).isArray() );
    OK( JSON::parse( "[ 1234 ]" )[0].isInt() );
    OK( JSON::parse( "[ 12345678901234 ]" )[0].isInt64() );
    OK( JSON::parse( "[ 1.123e3 ]" )[0].isDouble() );
    OK( JSON::parse( "[ -1234]" )[0].isInt() );
    OK( JSON::parse( "[-12345678901234]" )[0].isInt64() );
    OK( JSON::parse( "[-1.123e3]" )[0].isDouble() );

    for (int i = 100; --i >= 0; )
    {
        var v;

        if (i > 0)
            v = createRandomVar( 0 );

        const bool oneLine = r.next_bool();
        String asString( JSON::toString( v, oneLine ) );
        var parsed = JSON::parse( "[" + asString + "]" )[0];
        String parsedString( JSON::toString( parsed, oneLine ) );
        OK( asString.isNotEmpty() );
        IS( parsedString, asString );
    }

    StringPool::releaseInstance();
}
