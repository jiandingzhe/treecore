#include "treecore/GZIPDecompressorInputStream.h"
#include "treecore/GZIPCompressorOutputStream.h"
#include "treecore/MemoryInputStream.h"
#include "treecore/MemoryOutputStream.h"
#include "treecore/MT19937.h"

#include "TestFramework.h"

using namespace treecore;

void TestFramework::content()
{
    OK( "GZIP" );
    MT19937& rng = *MT19937::getInstance();

    for (int i = 100; --i >= 0; )
    {
        MemoryOutputStream original, compressed, uncompressed;

        {
            GZIPCompressorOutputStream zipper( &compressed, int( rng.next_uint64_in_range( 10 ) ), false );

            for (int j = int( rng.next_uint64_in_range( 100 ) ); --j >= 0; )
            {
                MemoryBlock data( size_t( rng.next_uint64_in_range( 2000 ) + 1 ) );

                for (int k = int( data.getSize() ); --k >= 0; )
                    data[k] = char( rng.next_uint64_in_range( 255 ) );

                original << data;
                zipper << data;
            }
        }

        {
            MemoryInputStream compressedInput( compressed.getData(), compressed.getDataSize(), false );
            GZIPDecompressorInputStream unzipper( compressedInput );

            uncompressed << unzipper;
        }

        IS( int( uncompressed.getDataSize() ),
            int( original.getDataSize() ) );

        if ( original.getDataSize() == uncompressed.getDataSize() )
            IS( memcmp( uncompressed.getData(),
                        original.getData(),
                        original.getDataSize() ), 0 );
    }

    MT19937::releaseInstance();
}

