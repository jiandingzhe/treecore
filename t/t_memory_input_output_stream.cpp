#include "treecore/MemoryInputStream.h"
#include "treecore/MemoryOutputStream.h"
#include "treecore/MT19937.h"

#include "treecore/TestFramework.h"

using namespace treecore;

static String createRandomWideCharString()
{
    MT19937& r = *MT19937::getInstance();
    treecore_wchar buffer [50] = { 0 };

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

void TestFramework::content()
{
    OK( "Basics" );
    MT19937& r = *MT19937::getInstance();

    int randomInt = int( r.next_uint64() );
    int64 randomInt64   = int64( r.next_uint64() );
    double randomDouble = r.next_double_yn();
    String randomString( createRandomWideCharString() );

    MemoryOutputStream mo;
    mo.writeInt( randomInt );
    mo.writeIntBigEndian( randomInt );
    mo.writeCompressedInt( randomInt );
    mo.writeString( randomString );
    mo.writeInt64( randomInt64 );
    mo.writeInt64BigEndian( randomInt64 );
    mo.writeDouble( randomDouble );
    mo.writeDoubleBigEndian( randomDouble );

    MemoryInputStream mi( mo.getData(), mo.getDataSize(), false );
    IS( mi.readInt(),             randomInt );
    IS( mi.readIntBigEndian(),    randomInt );
    IS( mi.readCompressedInt(),   randomInt );
    IS( mi.readString(),          randomString );
    IS( mi.readInt64(),           randomInt64 );
    IS( mi.readInt64BigEndian(),  randomInt64 );
    IS( mi.readDouble(),          randomDouble );
    IS( mi.readDoubleBigEndian(), randomDouble );
}
