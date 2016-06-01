#include "treecore/TestFramework.h"
#include "treecore/SimdFunc.h"

#include <limits>

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
    OK( "set by values" );
    SIMDType<16> a;
    simd_set_all<float, 16>( a, 1, 2, 3, 4 );
    IS( a.values_by_float[0], 1.0f );
    IS( a.values_by_float[1], 2.0f );
    IS( a.values_by_float[2], 3.0f );
    IS( a.values_by_float[3], 4.0f );

    {
        OK( "set all to same value" );
        SIMDType<16> value;

        simd_broadcast<int32, 16>( value, 42 );
        IS( value.values_by_i32[3], 42 );
        IS( value.values_by_i32[2], 42 );
        IS( value.values_by_i32[1], 42 );
        IS( value.values_by_i32[0], 42 );

        simd_broadcast<float>( value, 12.34f );
        IS( value.values_by_float[0], 12.34f );
        IS( value.values_by_float[1], 12.34f );
        IS( value.values_by_float[2], 12.34f );
        IS( value.values_by_float[3], 12.34f );

        simd_broadcast<double>( value, 56.78 );
        IS( value.values_by_double[0], 56.78 );
        IS( value.values_by_double[1], 56.78 );
    }

    {
        OK( "set value using pointer" );
        SIMDType<16> value;

        int32 data_i[4] = {100, 120, 246, 451};
        simd_set_all<int32>( value, data_i );
        IS( value.values_by_i32[0], data_i[0] );
        IS( value.values_by_i32[1], data_i[1] );
        IS( value.values_by_i32[2], data_i[2] );
        IS( value.values_by_i32[3], data_i[3] );

        int64 data_i64[2] = {1234567898765432, 4567891234567654};
        simd_set_all<int64>( value, data_i64 );
        IS( value.values_by_i64[0], data_i64[0] );
        IS( value.values_by_i64[1], data_i64[1] );

        float data_f[4] = {5.0f, 6.0f, 7.0f, 8.0f};
        simd_set_all<float>( value, data_f );
        IS( value.values_by_float[0], data_f[0] );
        IS( value.values_by_float[1], data_f[1] );
        IS( value.values_by_float[2], data_f[2] );
        IS( value.values_by_float[3], data_f[3] );

        double data_d[2] = {123.456, 789.123};
        simd_set_all<double>( value, data_d );
        IS( value.values_by_double[0], data_d[0] );
        IS( value.values_by_double[1], data_d[1] );
    }

    OK( "binary layout" );
    {
        float v0 = simd_get_one<0, float>( a );
        float v1 = simd_get_one<1, float>( a );
        float v2 = simd_get_one<2, float>( a );
        float v3 = simd_get_one<3, float>( a );
        IS( v0, a.values_by_float[0] );
        IS( v1, a.values_by_float[1] );
        IS( v2, a.values_by_float[2] );
        IS( v3, a.values_by_float[3] );

        OK( "sum" );
        float sum = simd_sum<float>( a );
        IS( sum, v0 + v1 + v2 + v3 );
    }

    OK( "add" );
    SIMDType<16> b;
    simd_set_all<float, 16>( b, 5, 6, 7, 8 );
    IS( b.values_by_float[0], 5.0f );
    IS( b.values_by_float[1], 6.0f );
    IS( b.values_by_float[2], 7.0f );
    IS( b.values_by_float[3], 8.0f );

    SIMDType<16> c;
    simd_add<float>( c, a, b );
    IS( c.values_by_float[0],  6.0f );
    IS( c.values_by_float[1],  8.0f );
    IS( c.values_by_float[2], 10.0f );
    IS( c.values_by_float[3], 12.0f );

    OK( "sub" );
    simd_sub<float>( c, a, b );
    IS( c.values_by_float[0], -4.0f );
    IS( c.values_by_float[1], -4.0f );
    IS( c.values_by_float[2], -4.0f );
    IS( c.values_by_float[3], -4.0f );

    OK( "mul" );
    simd_mul<float>( c, a, b );
    IS( c.values_by_float[0],  5.0f );
    IS( c.values_by_float[1], 12.0f );
    IS( c.values_by_float[2], 21.0f );
    IS( c.values_by_float[3], 32.0f );

    OK( "div" );
    simd_div<float>( c, a, b );
    IS( c.values_by_float[0], 1.0f / 5 );
    IS( c.values_by_float[1], 2.0f / 6 );
    IS( c.values_by_float[2], 3.0f / 7 );
    IS( c.values_by_float[3], 4.0f / 8 );

    OK( "convert float to int32" );
    {
        SIMDType<16> fv;
        SIMDType<16> iv;
        simd_set_all<float>( fv, 1.2f, 3.4f, 5.6f, 78.912f );
        simd_convert<int32, float>( iv, fv );
        IS( iv.values_by_i32[0],  1 );
        IS( iv.values_by_i32[1],  3 );
        IS( iv.values_by_i32[2],  5 );
        IS( iv.values_by_i32[3], 78 );
    }

    OK( "convert float to double" );
    {
        SIMDType<16> fv;
        SIMDType<16> dv;
        simd_set_all<float>( fv, 1.2f, 3.4f, 0.0f, 0.0f );
        simd_convert<double, float>( dv, fv );
        IS_EPSILON( dv.values_by_double[0], 1.2 );
        IS_EPSILON( dv.values_by_double[1], 3.4 );
    }

    OK( "convert int32 to float" );
    {
        SIMDType<16> iv;
        SIMDType<16> fv;
        simd_set_all<int32>( iv, 123, -456, 789, -422 );
        simd_convert<float, int32>( fv, iv );
        IS( fv.values_by_float[0], 123.0f );
        IS( fv.values_by_float[1],      -456.0f );
        IS( fv.values_by_float[2], 789.0f );
        IS( fv.values_by_float[3],      -422.0f );
    }
    OK( "convert int32 to double" );
    {
        SIMDType<16> iv;
        SIMDType<16> dv;
        simd_set_all<float>( iv, 123, -456, 0, 0 );
        simd_convert<double, float>( dv, iv );
        IS( dv.values_by_double[0],  123 );
        IS( dv.values_by_double[1], -456 );
    }

    OK( "convert double to float" );
    {
        SIMDType<16> dv;
        SIMDType<16> fv;
        simd_set_all<double>( dv, -1.26, 3.45 );
        simd_convert<float, double>( fv, dv );
        IS( fv.values_by_float[0],     -1.26f );
        IS( fv.values_by_float[1], 3.45f );
        IS( fv.values_by_float[2],  0.0f );
        IS( fv.values_by_float[3],  0.0f );
    }
    OK( "convert double to int32" );
    {
        SIMDType<16> dv;
        SIMDType<16> iv;
        simd_set_all<double>( dv, 12.26, 345.45 );
        simd_convert<int32, double>( iv, dv );
        IS( iv.values_by_i32[0],  12 );
        IS( iv.values_by_i32[1], 345 );
        IS( iv.values_by_i32[2],   0 );
        IS( iv.values_by_i32[3],   0 );
    }

    OK( "shuffle 3 2 1 0" );
    c = a;
    simd_shuffle<3, 2, 1, 0>( c );
    IS( c.values_by_float[0], 4.0f );
    IS( c.values_by_float[1], 3.0f );
    IS( c.values_by_float[2], 2.0f );
    IS( c.values_by_float[3], 1.0f );

    OK( "shuffle 1 0 3 2" );
    c = a;
    simd_shuffle<1, 0, 3, 2>( c );
    IS( c.values_by_float[0], 2.0f );
    IS( c.values_by_float[1], 1.0f );
    IS( c.values_by_float[2], 4.0f );
    IS( c.values_by_float[3], 3.0f );

    {
        OK( "XOR float" );
        SIMDType<16> result;
        SIMDType<16> input;
        SIMDType<16> mask;
        simd_set_all<float, 16>( input,      1.2f, 3.4f,       5.6f, 7.8f );
        simd_set_all<int32, 16>( mask, 0x80000000,    0, 0x80000000,    0 );
        simd_xor<float>( result, input, mask );
        IS( result.values_by_float[0],    -1.2f );
        IS( result.values_by_float[1], 3.4f );
        IS( result.values_by_float[2],    -5.6f );
        IS( result.values_by_float[3], 7.8f );
    }

    {
        OK( "complement" );
        SIMDType<16> value;
        simd_set_all<int32>( value, 0xffff0000, 0x00ff00ff, 0xf0f0f0f0, 0x0ff00ff0 );

        simd_cmpl<float>( value );
        IS( value.values_by_i32[0], int32( 0x0000ffff ) );
        IS( value.values_by_i32[1], int32( 0xff00ff00 ) );
        IS( value.values_by_i32[2], int32( 0x0f0f0f0f ) );
        IS( value.values_by_i32[3], int32( 0xf00ff00f ) );

        simd_cmpl<double>( value );
        IS( value.values_by_i32[0], int32( 0xffff0000 ) );
        IS( value.values_by_i32[1], int32( 0x00ff00ff ) );
        IS( value.values_by_i32[2], int32( 0xf0f0f0f0 ) );
        IS( value.values_by_i32[3], int32( 0x0ff00ff0 ) );

        simd_cmpl<int>( value );
        IS( value.values_by_i32[0], int32( 0x0000ffff ) );
        IS( value.values_by_i32[1], int32( 0xff00ff00 ) );
        IS( value.values_by_i32[2], int32( 0x0f0f0f0f ) );
        IS( value.values_by_i32[3], int32( 0xf00ff00f ) );
    }

    OK( "shift int16 right, filling with sign bit" );
    {
        SIMDType<16> value;
        simd_set_all<int16>( value, 74, -8765, -99, 6, 25139, 100, 37, -2 );
        simd_shift_right_sign<int16>( value, value, 3 );
        IS( value.values_by_i16[0],     9 );
        IS( value.values_by_i16[1], int16( 0xfbb8 ) );
        IS( value.values_by_i16[2], int16( 0xfff3 ) );
        IS( value.values_by_i16[3],     0 );
        IS( value.values_by_i16[4],  3142 );
        IS( value.values_by_i16[5],    12 );
        IS( value.values_by_i16[6],     4 );
        IS( value.values_by_i16[7], int16( 0xffff ) );
    }

    OK( "shift int32 right, filling with sign bit" );
    {
        SIMDType<16> value;
        simd_set_all<int32>( value, 4325634, -106321, 356009, -1256213 );
        simd_shift_right_sign<int32>( value, value, 5 );
        IS( value.values_by_i32[0], int32( 135176 ) );
        IS( value.values_by_i32[1], int32( 4294963973 ) );
        IS( value.values_by_i32[2], int32( 11125 ) );
        IS( value.values_by_i32[3], int32( 4294928039 ) );
    }

    OK( "shift int16 right, filling with zero" );
    {
        SIMDType<16> value;
        simd_set_all<int16>( value, 74, -8765, -99, 6, 25139, 100, 37, -2 );
        simd_shift_right_zero<int16>( value, value, 3 );
        IS( value.values_by_i16[0],    9 );
        IS( value.values_by_i16[1], 7096 );
        IS( value.values_by_i16[2], 8179 );
        IS( value.values_by_i16[3],    0 );
        IS( value.values_by_i16[4], 3142 );
        IS( value.values_by_i16[5],   12 );
        IS( value.values_by_i16[6],    4 );
        IS( value.values_by_i16[7], 8191 );
    }

    OK( "shift int32 right, filling with zero" );
    {
        SIMDType<16> value;
        simd_set_all<int32>( value, 4325634, -106321, 356009, -1256213 );
        simd_shift_right_zero<int32>( value, value, 5 );
        IS( value.values_by_i32[0], int32( 135176 ) );
        IS( value.values_by_i32[1], int32( 134214405 ) );
        IS( value.values_by_i32[2], int32( 11125 ) );
        IS( value.values_by_i32[3], int32( 134178471 ) );
    }

    OK( "shift int64 right, filling with zero" );
    {
        SIMDType<16> value;
        simd_set_all<int64>( value, 12345678765432, -98765432 );
        simd_shift_right_zero<int64>( value, value, 7 );
        IS( value.values_by_i64[0],        96450615354 );
        IS( value.values_by_i64[1], 144115188075084267 );
    }

    OK( "shift int16 left" );
    {
        SIMDType<16> value;
        simd_set_all<int16>( value, -1539, 3, 346, 920, 25, -3153, -44, 7 );
        simd_shift_left<int16>( value, value, 3 );
        IS( value.values_by_i16[0], int16( 0xcfe8 ) );
        IS( value.values_by_i16[1],    24 );
        IS( value.values_by_i16[2],  2768 );
        IS( value.values_by_i16[3],  7360 );
        IS( value.values_by_i16[4],   200 );
        IS( value.values_by_i16[5], int16( 0x9d78 ) );
        IS( value.values_by_i16[6], int16( 0xfea0 ) );
        IS( value.values_by_i16[7],    56 );
    }

    OK( "shift int32 left" );
    {
        SIMDType<16> value;
        simd_set_all<int32>( value, 12356, -543215, 444, 5246976 );
        simd_shift_left<int32>( value, value, 11 );
        IS( value.values_by_i32[0], int32( 25305088 ) );
        IS( value.values_by_i32[1], int32( 3182462976 ) );
        IS( value.values_by_i32[2], int32( 909312 ) );
        IS( value.values_by_i32[3], int32( 2155872256 ) );
    }

    OK( "shift int64 left" );
    {
        SIMDType<16> value;
        simd_set_all<int64>( value, 12345678765432, -98765432 );
        simd_shift_left<int64>( value, value, 7 );
        IS( value.values_by_i64[0], 1580246881975296 );
        IS( value.values_by_i64[1],     -12641975296 );
    }

    //
    // comparison functions
    //
    OK( "int8 comparison" );
    {
        SIMDType<16> value1;
        SIMDType<16> value2;
        SIMDType<16> result;
        simd_set_all<int8>( value1,
                            127, -3, 5, 54, 0, -128,  6, 93,
                            4, 30, 28, 3, -6, 25, 11, 90 );
        simd_set_all<int8>( value2,
                            -128, -4, 5, 55, 1, -127, 98, 93,
                            4, 30, 29, 2, -6, -5, 11, 20 );

        simd_cmp<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0 ) );
        IS( result.values_by_i8[1],  int8( 0 ) );
        IS( result.values_by_i8[2],  int8( 0xff ) );
        IS( result.values_by_i8[3],  int8( 0 ) );
        IS( result.values_by_i8[4],  int8( 0 ) );
        IS( result.values_by_i8[5],  int8( 0 ) );
        IS( result.values_by_i8[6],  int8( 0 ) );
        IS( result.values_by_i8[7],  int8( 0xff ) );
        IS( result.values_by_i8[8],  int8( 0xff ) );
        IS( result.values_by_i8[9],  int8( 0xff ) );
        IS( result.values_by_i8[10], int8( 0 ) );
        IS( result.values_by_i8[11], int8( 0 ) );
        IS( result.values_by_i8[12], int8( 0xff ) );
        IS( result.values_by_i8[13], int8( 0 ) );
        IS( result.values_by_i8[14], int8( 0xff ) );
        IS( result.values_by_i8[15], int8( 0 ) );

        simd_ncmp<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0xff ) );
        IS( result.values_by_i8[1],  int8( 0xff ) );
        IS( result.values_by_i8[2],  int8( 0 ) );
        IS( result.values_by_i8[3],  int8( 0xff ) );
        IS( result.values_by_i8[4],  int8( 0xff ) );
        IS( result.values_by_i8[5],  int8( 0xff ) );
        IS( result.values_by_i8[6],  int8( 0xff ) );
        IS( result.values_by_i8[7],  int8( 0 ) );
        IS( result.values_by_i8[8],  int8( 0 ) );
        IS( result.values_by_i8[9],  int8( 0 ) );
        IS( result.values_by_i8[10], int8( 0xff ) );
        IS( result.values_by_i8[11], int8( 0xff ) );
        IS( result.values_by_i8[12], int8( 0 ) );
        IS( result.values_by_i8[13], int8( 0xff ) );
        IS( result.values_by_i8[14], int8( 0 ) );
        IS( result.values_by_i8[15], int8( 0xff ) );

        simd_gt<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0xff ) );
        IS( result.values_by_i8[1],  int8( 0xff ) );
        IS( result.values_by_i8[2],  int8( 0 ) );
        IS( result.values_by_i8[3],  int8( 0 ) );
        IS( result.values_by_i8[4],  int8( 0 ) );
        IS( result.values_by_i8[5],  int8( 0 ) );
        IS( result.values_by_i8[6],  int8( 0 ) );
        IS( result.values_by_i8[7],  int8( 0 ) );
        IS( result.values_by_i8[8],  int8( 0 ) );
        IS( result.values_by_i8[9],  int8( 0 ) );
        IS( result.values_by_i8[10], int8( 0 ) );
        IS( result.values_by_i8[11], int8( 0xff ) );
        IS( result.values_by_i8[12], int8( 0 ) );
        IS( result.values_by_i8[13], int8( 0xff ) );
        IS( result.values_by_i8[14], int8( 0 ) );
        IS( result.values_by_i8[15], int8( 0xff ) );

        simd_ge<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0xff ) );
        IS( result.values_by_i8[1],  int8( 0xff ) );
        IS( result.values_by_i8[2],  int8( 0xff ) );
        IS( result.values_by_i8[3],  int8( 0 ) );
        IS( result.values_by_i8[4],  int8( 0 ) );
        IS( result.values_by_i8[5],  int8( 0 ) );
        IS( result.values_by_i8[6],  int8( 0 ) );
        IS( result.values_by_i8[7],  int8( 0xff ) );
        IS( result.values_by_i8[8],  int8( 0xff ) );
        IS( result.values_by_i8[9],  int8( 0xff ) );
        IS( result.values_by_i8[10], int8( 0 ) );
        IS( result.values_by_i8[11], int8( 0xff ) );
        IS( result.values_by_i8[12], int8( 0xff ) );
        IS( result.values_by_i8[13], int8( 0xff ) );
        IS( result.values_by_i8[14], int8( 0xff ) );
        IS( result.values_by_i8[15], int8( 0xff ) );

        simd_lt<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0 ) );
        IS( result.values_by_i8[1],  int8( 0 ) );
        IS( result.values_by_i8[2],  int8( 0 ) );
        IS( result.values_by_i8[3],  int8( 0xff ) );
        IS( result.values_by_i8[4],  int8( 0xff ) );
        IS( result.values_by_i8[5],  int8( 0xff ) );
        IS( result.values_by_i8[6],  int8( 0xff ) );
        IS( result.values_by_i8[7],  int8( 0 ) );
        IS( result.values_by_i8[8],  int8( 0 ) );
        IS( result.values_by_i8[9],  int8( 0 ) );
        IS( result.values_by_i8[10], int8( 0xff ) );
        IS( result.values_by_i8[11], int8( 0 ) );
        IS( result.values_by_i8[12], int8( 0 ) );
        IS( result.values_by_i8[13], int8( 0 ) );
        IS( result.values_by_i8[14], int8( 0 ) );
        IS( result.values_by_i8[15], int8( 0 ) );

        simd_le<int8>( result, value1, value2 );
        IS( result.values_by_i8[0],  int8( 0 ) );
        IS( result.values_by_i8[1],  int8( 0 ) );
        IS( result.values_by_i8[2],  int8( 0xff ) );
        IS( result.values_by_i8[3],  int8( 0xff ) );
        IS( result.values_by_i8[4],  int8( 0xff ) );
        IS( result.values_by_i8[5],  int8( 0xff ) );
        IS( result.values_by_i8[6],  int8( 0xff ) );
        IS( result.values_by_i8[7],  int8( 0xff ) );
        IS( result.values_by_i8[8],  int8( 0xff ) );
        IS( result.values_by_i8[9],  int8( 0xff ) );
        IS( result.values_by_i8[10], int8( 0xff ) );
        IS( result.values_by_i8[11], int8( 0 ) );
        IS( result.values_by_i8[12], int8( 0xff ) );
        IS( result.values_by_i8[13], int8( 0 ) );
        IS( result.values_by_i8[14], int8( 0xff ) );
        IS( result.values_by_i8[15], int8( 0 ) );
    }

    OK( "int16 comparison" );
    {
        SIMDType<16> value1;
        SIMDType<16> value2;
        SIMDType<16> result;
        simd_set_all<int16>( value1, 123, -432, 10,  0, 32767, -32768, -32768, 1 );
        simd_set_all<int16>( value2, 124, -432,  9, -1, 32766, -32768, -32767, 1 );

        simd_cmp<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0 ) );
        IS( result.values_by_i16[1], int16( 0xffff ) );
        IS( result.values_by_i16[2], int16( 0 ) );
        IS( result.values_by_i16[3], int16( 0 ) );
        IS( result.values_by_i16[4], int16( 0 ) );
        IS( result.values_by_i16[5], int16( 0xffff ) );
        IS( result.values_by_i16[6], int16( 0 ) );
        IS( result.values_by_i16[7], int16( 0xffff ) );

        simd_ncmp<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0xffff ) );
        IS( result.values_by_i16[1], int16( 0 ) );
        IS( result.values_by_i16[2], int16( 0xffff ) );
        IS( result.values_by_i16[3], int16( 0xffff ) );
        IS( result.values_by_i16[4], int16( 0xffff ) );
        IS( result.values_by_i16[5], int16( 0 ) );
        IS( result.values_by_i16[6], int16( 0xffff ) );
        IS( result.values_by_i16[7], int16( 0 ) );

        simd_gt<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0 ) );
        IS( result.values_by_i16[1], int16( 0 ) );
        IS( result.values_by_i16[2], int16( 0xffff ) );
        IS( result.values_by_i16[3], int16( 0xffff ) );
        IS( result.values_by_i16[4], int16( 0xffff ) );
        IS( result.values_by_i16[5], int16( 0 ) );
        IS( result.values_by_i16[6], int16( 0 ) );
        IS( result.values_by_i16[7], int16( 0 ) );

        simd_ge<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0 ) );
        IS( result.values_by_i16[1], int16( 0xffff ) );
        IS( result.values_by_i16[2], int16( 0xffff ) );
        IS( result.values_by_i16[3], int16( 0xffff ) );
        IS( result.values_by_i16[4], int16( 0xffff ) );
        IS( result.values_by_i16[5], int16( 0xffff ) );
        IS( result.values_by_i16[6], int16( 0 ) );
        IS( result.values_by_i16[7], int16( 0xffff ) );

        simd_lt<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0xffff ) );
        IS( result.values_by_i16[1], int16( 0 ) );
        IS( result.values_by_i16[2], int16( 0 ) );
        IS( result.values_by_i16[3], int16( 0 ) );
        IS( result.values_by_i16[4], int16( 0 ) );
        IS( result.values_by_i16[5], int16( 0 ) );
        IS( result.values_by_i16[6], int16( 0xffff ) );
        IS( result.values_by_i16[7], int16( 0 ) );

        simd_le<int16>( result, value1, value2 );
        IS( result.values_by_i16[0], int16( 0xffff ) );
        IS( result.values_by_i16[1], int16( 0xffff ) );
        IS( result.values_by_i16[2], int16( 0 ) );
        IS( result.values_by_i16[3], int16( 0 ) );
        IS( result.values_by_i16[4], int16( 0 ) );
        IS( result.values_by_i16[5], int16( 0xffff ) );
        IS( result.values_by_i16[6], int16( 0xffff ) );
        IS( result.values_by_i16[7], int16( 0xffff ) );
    }

    OK( "int32 comparison" );
    {
        SIMDType<16> value1;
        SIMDType<16> value2;
        SIMDType<16> result;
        simd_set_all<int32>( value1, int32( 0x7fffffff ), int32( 2 ), int32( 0xffffffff ), int32( 0x80000000 ) );
        simd_set_all<int32>( value2, int32( 0x80000000 ), int32( 2 ), int32( 1 ),          int32( 0xffffffff ) );

        simd_cmp<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0xffffffff ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_ncmp<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );

        simd_gt<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_ge<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0xffffffff ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_lt<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );

        simd_le<int32>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0xffffffff ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );
    }

    OK( "float comparison" );
    {
        SIMDType<16> value1;
        SIMDType<16> value2;
        SIMDType<16> result;
        simd_set_all<float>( value1, 1.2001f, std::numeric_limits<float>::quiet_NaN(), 5.6f, -7.8f );
        simd_set_all<float>( value2,    1.2f, std::numeric_limits<float>::quiet_NaN(), 5.6f, -7.799f );

        simd_cmp<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_ncmp<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0xffffffff ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );

        simd_gt<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_ge<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0xffffffff ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0 ) );

        simd_lt<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0 ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );

        simd_le<float>( result, value1, value2 );
        IS( result.values_by_i32[0], int32( 0 ) );
        IS( result.values_by_i32[1], int32( 0 ) );
        IS( result.values_by_i32[2], int32( 0xffffffff ) );
        IS( result.values_by_i32[3], int32( 0xffffffff ) );
    }

    OK( "double comparison" );
    {
        SIMDType<16> value1;
        SIMDType<16> value2;
        SIMDType<16> result;

        simd_set_all<double>( value1, 1.2001, std::numeric_limits<double>::quiet_NaN() );
        simd_set_all<double>( value2,    1.2, std::numeric_limits<double>::infinity() );

        simd_cmp<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_ncmp<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0xffffffffffffffff ) );

        simd_gt<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_ge<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_lt<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_le<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_set_all<double>( value1, 3.4, 5.6 );
        simd_set_all<double>( value2, 3.4, std::numeric_limits<double>::infinity() );

        simd_cmp<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_ncmp<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0xffffffffffffffff ) );

        simd_gt<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_ge<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0 ) );

        simd_lt<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0 ) );
        IS( result.values_by_i64[1], int32( 0xffffffffffffffff ) );

        simd_le<double>( result, value1, value2 );
        IS( result.values_by_i64[0], int32( 0xffffffffffffffff ) );
        IS( result.values_by_i64[1], int32( 0xffffffffffffffff ) );
    }

    OK( "equal operator int16" );
    {
        SIMDType<16> a, b;
        simd_set_all<int16>( a, 1, 2, 3, 4, 5, 6, 7, 8 );
        simd_set_all<int16>( b, 1, 2, 3, 4, 5, 6, 7, 8 );
        OK( simd_equal<int16>( a, b ) );

        simd_set_all<int16>( b, 1, 2, 3, 4, 5, 6, 7, 9 );
        OK( !simd_equal<int16>( a, b ) );

        simd_set_all<int16>( b, 1, 2, 3, 3, 5, 6, 7, 8 );
        OK( !simd_equal<int16>( a, b ) );

        simd_set_all<int16>( b, 1, 0, 3, 4, 3, 6, 7, 8 );
        OK( !simd_equal<int16>( a, b ) );

        simd_set_all<int16>( b, 1, 2, 3, 4, 5, 2, 5, 8 );
        OK( !simd_equal<int16>( a, b ) );
    }

    OK( "equal operator int32" );
    {
        SIMDType<16> a, b;
        simd_set_all<int32>( a, 1, 2, 3, 4 );
        simd_set_all<int32>( b, 1, 2, 3, 4 );
        OK( simd_equal<int32>( a, b ) );

        simd_set_all<int32>( b, 3, 2, 3, 4 );
        OK( !simd_equal<int32>( a, b ) );

        simd_set_all<int32>( b, 1, 2, 2, 4 );
        OK( !simd_equal<int32>( a, b ) );
    }

    OK( "equal operator float" );
    {
        SIMDType<16> a, b;
        simd_set_all<float>( a, 1.2f, 3.4f, 5.6f, 7.8f );
        simd_set_all<float>( b, 1.2f, 3.4f, 5.6f, 7.8f );
        OK( simd_equal<float>( a, b ) );

        simd_set_all<float>( b, 1.1f, 3.4f, 5.6f, 7.8f );
        OK( !simd_equal<float>( a, b ) );

        simd_set_all<float>( b, 1.2f, 3.4f, 5.6f, 7.7f );
        OK( !simd_equal<float>( a, b ) );

        simd_set_all<float>( b, 0.0f, 3.4f, 0.0f, 7.8f );
        OK( !simd_equal<float>( a, b ) );
    }

    OK( "equal operator double" );
    {
        SIMDType<16> a, b;
        simd_set_all<double>( a, 1.2, 123.456 );
        simd_set_all<double>( b, 1.2, 123.456 );
        OK( simd_equal<double>( a, b ) );

        simd_set_all<double>( b, 1.2001, 123.456 );
        OK( !simd_equal<double>( a, b ) );

        simd_set_all<double>( b, 1.3, 123.4567 );
        OK( !simd_equal<double>( a, b ) );
    }
}
