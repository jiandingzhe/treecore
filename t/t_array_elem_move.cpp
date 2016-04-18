#include "treecore/TestFramework.h"

#include <treecore/Array.h>

struct MovableObj
{
    MovableObj( float value ): value( value ) {}

    MovableObj( MovableObj&& peer ): value( peer.value )
    {
        peer.value = std::numeric_limits<float>::signaling_NaN();
    }

    MovableObj( const MovableObj& ) = delete;
    MovableObj& operator = ( const MovableObj& ) = delete;

    float value = std::numeric_limits<float>::signaling_NaN();
};

typedef treecore::Array<MovableObj> TestArray;

void TestFramework::content()
{
    TestArray array;

    {
        MovableObj obj( 1.2f );
        array.add( std::move( obj ) );
        OK( std::isnan( obj.value ) );
        IS( array.size(),      1 );
        IS( array[0].value, 1.2f );
    }

    {
        MovableObj obj( 3.4f );
        array.insert( 0, std::move( obj ) );
        OK( std::isnan( obj.value ) );
        IS( array.size(),      2 );
        IS( array[0].value, 3.4f );
        IS( array[1].value, 1.2f );
    }

    {
        MovableObj obj( 5.6f );
        array.insert( 2, std::move( obj ) );
        OK( std::isnan( obj.value ) );
        IS( array.size(),      3 );
        IS( array[0].value, 3.4f );
        IS( array[1].value, 1.2f );
        IS( array[2].value, 5.6f );
    }
}
