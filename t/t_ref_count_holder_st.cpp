#include "treecore/RefCountObject.h"
#include "treecore/RefCountHolder.h"

#include "treecore/TestFramework.h"

using namespace treecore;

struct TestType: public RefCountObject
{
    TestType( float foo, int16 bar, bool& living_mark ): foo( foo ), bar( bar ), living( living_mark )
    {
        living = true;
    }

    virtual ~TestType()
    {
        living = false;
    }

    float foo = 0.0f;
    int16 bar = 0;
    bool& living;
};

typedef RefCountHolder<TestType> Holder;

void TestFramework::content()
{
    bool a_living = false;
    TestType* a   = new TestType( 1.2f, 34, a_living );
    bool b_living = false;
    TestType* b   = new TestType( 5.6f, 78, b_living );

    IS( a->get_ref_count(), 0 );
    OK( a_living );
    IS( b->get_ref_count(), 0 );
    OK( b_living );

    // set value to holder
    Holder holder1 = a;
    IS( holder1.get(),      a );
    IS( a->get_ref_count(), 1 );

    // set value again
    // a should be still living
    holder1 = a;
    IS( holder1.get(),      a );
    IS( a->get_ref_count(), 1 );
    OK( a_living );

    // set to another holder
    Holder holder2 = a;
    IS( holder2.get(),      a );
    IS( a->get_ref_count(), 2 );
    OK( a_living );

    // set holder with another holder holding same value
    holder1 = holder2;
    IS( holder1.get(),      a );
    IS( holder2.get(),      a );
    IS( a->get_ref_count(), 2 );
    OK( a_living );

    // set holder with different value
    holder1 = b;
    IS( holder1.get(),      b );
    IS( holder2.get(),      a );
    IS( a->get_ref_count(), 1 );
    IS( b->get_ref_count(), 1 );
    OK( a_living );
    OK( b_living );

    // set holder with another holder holding different value
    holder2 = holder1;
    IS( holder1.get(), b );
    IS( holder2.get(), b );
    OK( b_living );
    IS( b->get_ref_count(), 2 );
    OK( !a_living );

    // set holder using nullptr
    holder1 = nullptr;
    OK( holder1.get() == nullptr );
    OK( b_living );
    IS( b->get_ref_count(), 1 );
}
