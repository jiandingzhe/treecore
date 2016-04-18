#include "treecore/RefCountSingleton.h"
#include "treecore/AtomicObject.h"
#include "treecore/Thread.h"
#include "treecore/MT19937.h"
#include "treecore/TestFramework.h"

#define NUM_ITER 100000
#define NUM_THREAD 10

using namespace treecore;

struct FooBar: public RefCountObject, public RefCountSingleton<FooBar>
{
    FooBar()
        : id( seed++ )
        , foo( float(id) + 1.2f )
        , bar( float(id) + 3.4f )
        , is_living(1)
        , value( 0 )
    {}

    void touch( uint64 value )
    {
        if (!is_living)
        {
            treecore_assert_false;
            abort();
        }
        this->value = value;
    }

    virtual ~FooBar()
    {
        if (get_ref_count() != 0)
        {
            treecore_assert_false;
            abort();
        }

        if (!is_living)
        {
            treecore_assert_false;
            abort();
        }

        is_living = 0;
    }

    int64 id;
    AtomicObject<uint64> value;
    float foo;
    float bar;
    AtomicObject<int32>  is_living;

    static AtomicObject<int64> seed;
};

AtomicObject<int64> FooBar::seed( 0 );

struct TestThread: public treecore::Thread
{
    TestThread( int thread_idx )
        : treecore::Thread( treecore::String( thread_idx ) )
        , prng( uint64( thread_idx ) )
    {}

    void run() override
    {
        Array<RefCountHolder<FooBar> > fuck;

        for (int iter = 0; iter < NUM_ITER; iter++)
        {
            int task = int( prng.next_uint64_in_range( 3 ) );

            switch (task)
            {
            case 0:
                fuck.clear();
                FooBar::releaseInstance();
                break;
            default:
                fuck.add( FooBar::getInstance() );
                break;
            }
        }

        fuck.clear();
    }

    MT19937 prng;
};

void TestFramework::content()
{
    Array<TestThread*> threads;
    for (int i_thread = 0; i_thread < NUM_THREAD; i_thread++)
        threads.add( new TestThread( i_thread ) );

    for (int i_thread = 0; i_thread < NUM_THREAD; i_thread++)
        threads[i_thread]->startThread();

    for (int i_thread = 0; i_thread < NUM_THREAD; i_thread++)
        threads[i_thread]->waitForThreadToExit( -1 );

    for (int i_thread = 0; i_thread < NUM_THREAD; i_thread++)
        delete threads[i_thread];

    FooBar::releaseInstance();
}
