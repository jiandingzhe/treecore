#include "treecore/TestFramework.h"
#include "treecore/SimdObject.h"

#include <limits>

using treecore::int32;

typedef treecore::SimdObject<float, 4> ftype;
typedef treecore::SimdObject<treecore::int32, 4> itype;

void TestFramework::content( int argc, char** argv )
{
    {
        ftype obj;
        IS( obj.get<0>(),  0.0f );
        IS( obj.get<1>(),  0.0f );
        IS( obj.get<2>(),  0.0f );
        IS( obj.get<3>(),  0.0f );

        obj = 1.1f;
        IS( obj.get<0>(),  1.1f );
        IS( obj.get<1>(),  1.1f );
        IS( obj.get<2>(),  1.1f );
        IS( obj.get<3>(),  1.1f );

        ftype obj2( 1.2f, 3.4f, 5.6f, 7.8f );
        IS( obj2.get<0>(), 1.2f );
        IS( obj2.get<1>(), 3.4f );
        IS( obj2.get<2>(), 5.6f );
        IS( obj2.get<3>(), 7.8f );

        obj = obj2;
        IS( obj.get<0>(),  1.2f );
        IS( obj.get<1>(),  3.4f );
        IS( obj.get<2>(),  5.6f );
        IS( obj.get<3>(),  7.8f );

        obj.set_all( 111.111f, 222.222f, 333.333f, 444.444f );
        IS( obj.get<0>(), 111.111f );
        IS( obj.get<1>(), 222.222f );
        IS( obj.get<2>(), 333.333f );
        IS( obj.get<3>(), 444.444f );
    }

    OK( "unary float arithmetic operations" );
    {
        OK( "add" );
        ftype obj;
        obj += ftype( 234.5f, 678.9f, 111.1f, 123.4f );
        IS( obj.get<0>(), 234.5f );
        IS( obj.get<1>(), 678.9f );
        IS( obj.get<2>(), 111.1f );
        IS( obj.get<3>(), 123.4f );

        OK( "sub" );
        obj -= ftype( 100.0f, 222.2f, 50.0f, 23.3f );
        IS_EPSILON( obj.get<0>(), 134.5f );
        IS_EPSILON( obj.get<1>(), 456.7f );
        IS_EPSILON( obj.get<2>(),  61.1f );
        IS_EPSILON( obj.get<3>(), 100.1f );

        OK( "mul" );
        obj *= ftype( 1.2f, 3.4f, 5.6f, 7.8f );
        IS_EPSILON( obj.get<0>(),   161.4f );
        IS_EPSILON( obj.get<1>(), 1552.78f );
        IS_EPSILON( obj.get<2>(),  342.16f );
        IS_EPSILON( obj.get<3>(),  780.78f );

        OK( "div" );
        obj /= ftype( 2.0f, 3.0f, 5.0f, 7.0f );
        IS_EPSILON( obj.get<0>(),     80.7f );
        IS_EPSILON( obj.get<1>(), 517.5933f );
        IS_EPSILON( obj.get<2>(),   68.432f );
        IS_EPSILON( obj.get<3>(),   111.54f );
    }

    OK( "unary logic operations" );
    {
        OK( "and" );
        itype obj( 0xffff0000, 0xf0f0f0f0, 0xff00ff00, 0x0ff00ff0 );
        obj &= itype( 0x000ff000, 0xff00ff00, 0x0ff00ff0, 0xffff0000 );
        IS( obj.get<0>(), 0x000f0000 );
        IS( obj.get<1>(), 0xf000f000 );
        IS( obj.get<2>(), 0x0f000f00 );
        IS( obj.get<3>(), 0x0ff00000 );

        OK( "or" );
        obj |= itype( 0x000fff00, 0x0ff00000, 0xff00000f, 0x0000ff00 );
        IS( obj.get<0>(), 0x000fff00 );
        IS( obj.get<1>(), 0xfff0f000 );
        IS( obj.get<2>(), 0xff000f0f );
        IS( obj.get<3>(), 0x0ff0ff00 );

        OK( "xor" );
        obj ^= itype( 0x00ff00ff, 0xffff0000, 0x0000ffff, 0xf0f0f0f0 );
        IS( obj.get<0>(), 0x00f0ffff );
        IS( obj.get<1>(), 0x000ff000 );
        IS( obj.get<2>(), 0xff00f0f0 );
        IS( obj.get<3>(), 0xff000ff0 );

        OK( "complement" );
        itype result = ~obj;
        IS( result.get<0>(), 0xff0f0000 );
        IS( result.get<1>(), 0xfff00fff );
        IS( result.get<2>(), 0x00ff0f0f );
        IS( result.get<3>(), 0x00fff00f );
    }

    OK( "binary float arithmetic operators" );
    {
        OK( "add" );
        ftype result = ftype( 1.2f, 3.4f, 5.6f, 7.8f ) + ftype( 1.1f, 2.2f, 3.3f, 4.4f );
        IS_EPSILON( result.get<0>(),  2.3f );
        IS_EPSILON( result.get<1>(),  5.6f );
        IS_EPSILON( result.get<2>(),  8.9f );
        IS_EPSILON( result.get<3>(), 12.2f );
    }
    {
        OK( "sub" );
        ftype result = ftype( 555.444f, 222.333f, 111.111f, 345.123f ) - ftype( 666.666f, 555.555f, 444.444f, 333.333f );
        IS_EPSILON( result.get<0>(),      -111.222f );
        IS_EPSILON( result.get<1>(),      -333.222f );
        IS_EPSILON( result.get<2>(),      -333.333f );
        IS_EPSILON( result.get<3>(), 11.79f );
    }
    {
        OK( "mul" );
        ftype result = ftype( 1.2f, 3.4f, 5.6f, 7.8f ) * ftype( 1.1f, 2.2f, 3.3f, 4.4f );
        IS_EPSILON( result.get<0>(),  1.32f );
        IS_EPSILON( result.get<1>(),  7.48f );
        IS_EPSILON( result.get<2>(), 18.48f );
        IS_EPSILON( result.get<3>(), 34.32f );
    }
    {
        OK( "div" );
        ftype result = ftype( 1.2f, 3.4f, 5.6f, 7.8f ) / ftype( 1.1f, 2.2f, 3.3f, 4.4f );
        IS_EPSILON( result.get<0>(), 1.090909f );
        IS_EPSILON( result.get<1>(), 1.545455f );
        IS_EPSILON( result.get<2>(), 1.696970f );
        IS_EPSILON( result.get<3>(), 1.772727f );
    }

    OK( "binary logic operations" );
    {
        itype result;

        OK( "and" );
        result = itype( 0xffff0000, 0xf0f0f0f0, 0xff00ff00, 0x0ff00ff0 ) & itype( 0x000ff000, 0xff00ff00, 0x0ff00ff0, 0xffff0000 );
        IS( result.get<0>(), 0x000f0000 );
        IS( result.get<1>(), 0xf000f000 );
        IS( result.get<2>(), 0x0f000f00 );
        IS( result.get<3>(), 0x0ff00000 );

        OK( "or" );
        result = itype( 0x000f0000, 0xf000f000, 0x0f000f00, 0x0ff00000 ) | itype( 0x000fff00, 0x0ff00000, 0xff00000f, 0x0000ff00 );
        IS( result.get<0>(), 0x000fff00 );
        IS( result.get<1>(), 0xfff0f000 );
        IS( result.get<2>(), 0xff000f0f );
        IS( result.get<3>(), 0x0ff0ff00 );

        OK( "xor" );
        result = itype( 0x000fff00, 0xfff0f000, 0xff000f0f, 0x0ff0ff00 ) ^ itype( 0x00ff00ff, 0xffff0000, 0x0000ffff, 0xf0f0f0f0 );
        IS( result.get<0>(), 0x00f0ffff );
        IS( result.get<1>(), 0x000ff000 );
        IS( result.get<2>(), 0xff00f0f0 );
        IS( result.get<3>(), 0xff000ff0 );
    }

    OK( "masking of float and int" );
    {
        ftype result;
        result = ftype( 111.222f, 333.444f, 555.666f, 777.888f ) & itype( 0xffffffff, 0x00000000, 0xffffffff, 0x00000000 );
        IS( result.get<0>(), 111.222f );
        IS( result.get<1>(),     0.0f );
        IS( result.get<2>(), 555.666f );
        IS( result.get<3>(),     0.0f );
    }

    OK( "float comparison" );
    {
        ftype value1( 111.222f, 333.144f, std::numeric_limits<float>::infinity(), 777.898f );
        ftype value2( 111.222f, 333.444f, 555.666f, 777.888f );
        itype result;

        result = (value1 == value2);
        IS( result.get<0>(), int32( 0xffffffff ) );
        IS( result.get<1>(), int32( 0 ) );
        IS( result.get<2>(), int32( 0 ) );
        IS( result.get<3>(), int32( 0 ) );

        result = (value1 > value2);
        IS( result.get<0>(), int32( 0 ) );
        IS( result.get<1>(), int32( 0 ) );
        IS( result.get<2>(), int32( 0xffffffff ) );
        IS( result.get<3>(), int32( 0xffffffff ) );

        result = (value1 >= value2);
        IS( result.get<0>(), int32( 0xffffffff ) );
        IS( result.get<1>(), int32( 0 ) );
        IS( result.get<2>(), int32( 0xffffffff ) );
        IS( result.get<3>(), int32( 0xffffffff ) );

        result = (value1 < value2);
        IS( result.get<0>(), int32( 0 ) );
        IS( result.get<1>(), int32( 0xffffffff ) );
        IS( result.get<2>(), int32( 0 ) );
        IS( result.get<3>(), int32( 0 ) );

        result = (value1 <= value2);
        IS( result.get<0>(), int32( 0xffffffff ) );
        IS( result.get<1>(), int32( 0xffffffff ) );
        IS( result.get<2>(), int32( 0 ) );
        IS( result.get<3>(), int32( 0 ) );
    }

}

