#include "TestFramework.h"
#include "treecore/SimdFunc.h"

using namespace treecore;

void TestFramework::content()
{
    OK("set by values");
    SIMDType<16> a;
    simd_set_all<float, 16>(a, 1, 2, 3, 4);
    IS(a.values_by_float[0], 1.0f);
    IS(a.values_by_float[1], 2.0f);
    IS(a.values_by_float[2], 3.0f);
    IS(a.values_by_float[3], 4.0f);

    {
        OK("set all to same value");
        SIMDType<16> value;

        simd_broadcast<int32, 16>(value, 42);
        IS(value.values_by_i32[3], 42);
        IS(value.values_by_i32[2], 42);
        IS(value.values_by_i32[1], 42);
        IS(value.values_by_i32[0], 42);

        simd_broadcast<float>(value, 12.34f);
        IS(value.values_by_float[0], 12.34f);
        IS(value.values_by_float[1], 12.34f);
        IS(value.values_by_float[2], 12.34f);
        IS(value.values_by_float[3], 12.34f);

        simd_broadcast<double>(value, 56.78);
        IS(value.values_by_double[0], 56.78);
        IS(value.values_by_double[1], 56.78);
    }

    {
        OK("set value using pointer");
        SIMDType<16> value;

        int32 data_i[4] = {100, 120, 246, 451};
        simd_set_all<int32>(value, data_i);
        IS(value.values_by_i32[0], data_i[0]);
        IS(value.values_by_i32[1], data_i[1]);
        IS(value.values_by_i32[2], data_i[2]);
        IS(value.values_by_i32[3], data_i[3]);

        float data_f[4] = {5.0f, 6.0f, 7.0f, 8.0f};
        simd_set_all<float>(value, data_f);
        IS(value.values_by_float[0], data_f[0]);
        IS(value.values_by_float[1], data_f[1]);
        IS(value.values_by_float[2], data_f[2]);
        IS(value.values_by_float[3], data_f[3]);

        double data_d[2] = {123.456, 789.123};
        simd_set_all<double>(value, data_d);
        IS(value.values_by_double[0], data_d[0]);
        IS(value.values_by_double[1], data_d[1]);
    }

    OK("binary layout");
    {
        float v0 = simd_get_one<0, float>(a);
        float v1 = simd_get_one<1, float>(a);
        float v2 = simd_get_one<2, float>(a);
        float v3 = simd_get_one<3, float>(a);
        IS(v0, a.values_by_float[0]);
        IS(v1, a.values_by_float[1]);
        IS(v2, a.values_by_float[2]);
        IS(v3, a.values_by_float[3]);

        OK("sum");
        float sum = simd_sum<float>(a);
        IS(sum, v0 + v1 + v2 + v3);
    }

    OK("add");
    SIMDType<16> b;
    simd_set_all<float, 16>(b, 5, 6, 7, 8);
    IS(b.values_by_float[0], 5.0f);
    IS(b.values_by_float[1], 6.0f);
    IS(b.values_by_float[2], 7.0f);
    IS(b.values_by_float[3], 8.0f);

    SIMDType<16> c;
    simd_add<float>(c, a, b);
    IS(c.values_by_float[0], 6.0f);
    IS(c.values_by_float[1], 8.0f);
    IS(c.values_by_float[2], 10.0f);
    IS(c.values_by_float[3], 12.0f);

    OK("sub");
    simd_sub<float>(c, a, b);
    IS(c.values_by_float[0], -4.0f);
    IS(c.values_by_float[1], -4.0f);
    IS(c.values_by_float[2], -4.0f);
    IS(c.values_by_float[3], -4.0f);

    OK("mul");
    simd_mul<float>(c, a, b);
    IS(c.values_by_float[0],  5.0f);
    IS(c.values_by_float[1], 12.0f);
    IS(c.values_by_float[2], 21.0f);
    IS(c.values_by_float[3], 32.0f);

    OK("div");
    simd_div<float>(c, a, b);
    IS(c.values_by_float[0], 1.0f/5);
    IS(c.values_by_float[1], 2.0f/6);
    IS(c.values_by_float[2], 3.0f/7);
    IS(c.values_by_float[3], 4.0f/8);

    OK("shuffle 3 2 1 0");
    c = a;
    simd_shuffle<3, 2, 1, 0>(c);
    IS(c.values_by_float[0], 4.0f);
    IS(c.values_by_float[1], 3.0f);
    IS(c.values_by_float[2], 2.0f);
    IS(c.values_by_float[3], 1.0f);

    OK("shuffle 1 0 3 2");
    c = a;
    simd_shuffle<1, 0, 3, 2>(c);
    IS(c.values_by_float[0], 2.0f);
    IS(c.values_by_float[1], 1.0f);
    IS(c.values_by_float[2], 4.0f);
    IS(c.values_by_float[3], 3.0f);

    {
        OK("XOR float");
        SIMDType<16> result;
        SIMDType<16> input;
        SIMDType<16> mask;
        simd_set_all<float, 16>(input, 1.2, 3.4, 5.6, 7.8);
        simd_set_all<int32, 16>(mask, 0x80000000, 0, 0x80000000, 0);
        simd_xor<float>(result, input, mask);
        IS(result.values_by_float[0], -1.2f);
        IS(result.values_by_float[1], 3.4f);
        IS(result.values_by_float[2], -5.6f);
        IS(result.values_by_float[3], 7.8f);
    }

    {
        OK("complement");
        SIMDType<16> value;
        simd_set_all<int32>(value, 0xffff0000, 0x00ff00ff, 0xf0f0f0f0, 0x0ff00ff0);

        simd_cmpl<float>(value);
        IS(value.values_by_i32[0], int32(0x0000ffff));
        IS(value.values_by_i32[1], int32(0xff00ff00));
        IS(value.values_by_i32[2], int32(0x0f0f0f0f));
        IS(value.values_by_i32[3], int32(0xf00ff00f));

        simd_cmpl<double>(value);
        IS(value.values_by_i32[0], int32(0xffff0000));
        IS(value.values_by_i32[1], int32(0x00ff00ff));
        IS(value.values_by_i32[2], int32(0xf0f0f0f0));
        IS(value.values_by_i32[3], int32(0x0ff00ff0));

        simd_cmpl<int>(value);
        IS(value.values_by_i32[0], int32(0x0000ffff));
        IS(value.values_by_i32[1], int32(0xff00ff00));
        IS(value.values_by_i32[2], int32(0x0f0f0f0f));
        IS(value.values_by_i32[3], int32(0xf00ff00f));
    }

    OK("equal operator int16");
    {
        SIMDType<16> a, b;
        simd_set_all<int16>(a, 1, 2, 3, 4, 5, 6, 7, 8);
        simd_set_all<int16>(b, 1, 2, 3, 4, 5, 6, 7, 8);
        OK(simd_equal<int16>(a, b));

        simd_set_all<int16>(b, 1, 2, 3, 4, 5, 6, 7, 9);
        OK(! simd_equal<int16>(a, b));

        simd_set_all<int16>(b, 1, 2, 3, 3, 5, 6, 7, 8);
        OK(! simd_equal<int16>(a, b));

        simd_set_all<int16>(b, 1, 0, 3, 4, 3, 6, 7, 8);
        OK(! simd_equal<int16>(a, b));

        simd_set_all<int16>(b, 1, 2, 3, 4, 5, 2, 5, 8);
        OK(! simd_equal<int16>(a, b));
    }

    OK("equal operator int32");
    {
        SIMDType<16> a, b;
        simd_set_all<int32>(a, 1, 2, 3, 4);
        simd_set_all<int32>(b, 1, 2, 3, 4);
        OK(simd_equal<int32>(a, b));

        simd_set_all<int32>(b, 3, 2, 3, 4);
        OK(! simd_equal<int32>(a, b));

        simd_set_all<int32>(b, 1, 2, 2, 4);
        OK(! simd_equal<int32>(a, b));
    }

    OK("equal operator float");
    {
        SIMDType<16> a, b;
        simd_set_all<float>(a, 1.2, 3.4, 5.6, 7.8);
        simd_set_all<float>(b, 1.2, 3.4, 5.6, 7.8);
        OK(simd_equal<float>(a, b));

        simd_set_all<float>(b, 1.1, 3.4, 5.6, 7.8);
        OK(! simd_equal<float>(a, b));

        simd_set_all<float>(b, 1.2, 3.4, 5.6, 7.7);
        OK(! simd_equal<float>(a, b));

        simd_set_all<float>(b, 0.0, 3.4, 0.0, 7.8);
        OK(!simd_equal<float>(a, b));
    }
}
