#include "treecore/AbstractFifo.h"
#include "treecore/MT19937.h"
#include "treecore/Thread.h"

#include "treecore/TestFramework.h"

using namespace treecore;

class WriteThread: public Thread
{
public:
    WriteThread ( AbstractFifo& f, int* b )
        : Thread( "fifo writer" ), fifo( f ), buffer( b )
    {
        startThread();
    }

    ~WriteThread()
    {
        stopThread( 5000 );
    }

    void run()
    {
        int n = 0;

        while ( !threadShouldExit() )
        {
            int num = int( rng.next_uint64_in_range( 2000 ) ) + 1;

            int start1, size1, start2, size2;
            fifo.prepareToWrite( num, start1, size1, start2, size2 );

            treecore_assert( size1 >= 0 && size2 >= 0 );
            treecore_assert( size1 == 0 || ( start1 >= 0 && start1 < fifo.getTotalSize() ) );
            treecore_assert( size2 == 0 || ( start2 >= 0 && start2 < fifo.getTotalSize() ) );

            for (int i = 0; i < size1; ++i)
                buffer [start1 + i] = n++;

            for (int i = 0; i < size2; ++i)
                buffer [start2 + i] = n++;

            fifo.finishedWrite( size1 + size2 );
        }
    }

private:
    AbstractFifo& fifo;
    int* buffer;
    MT19937 rng;
};

void TestFramework::content()
{
    OK( "AbstractFifo" );

    int buffer [5000];
    AbstractFifo fifo( numElementsInArray( buffer ) );

    WriteThread writer( fifo, buffer );

    int n = 0;
    MT19937 r;

    for (int count = 100000; --count >= 0; )
    {
        int num = int( r.next_uint64_in_range( 6000 ) ) + 1;

        int start1, size1, start2, size2;
        fifo.prepareToRead( num, start1, size1, start2, size2 );

        if ( !(size1 >= 0 && size2 >= 0)
             && ( size1 == 0 || ( start1 >= 0 && start1 < fifo.getTotalSize() ) )
             && ( size2 == 0 || ( start2 >= 0 && start2 < fifo.getTotalSize() ) ) )
        {
            ok( false, "prepareToRead returned -ve values" );
            break;
        }

        bool failed = false;

        for (int i = 0; i < size1; ++i)
            failed = (buffer [start1 + i] != n++) || failed;

        for (int i = 0; i < size2; ++i)
            failed = (buffer [start2 + i] != n++) || failed;

        if (failed)
        {
            ok( false, "read values were incorrect" );
            break;
        }

        fifo.finishedRead( size1 + size2 );
    }
}
