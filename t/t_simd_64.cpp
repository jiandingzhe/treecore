#include "treecore/TestFramework.h"
#include "treecore/SimdFunc.h"

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
#if ! (defined TREECORE_COMPILER_MSVC && TREECORE_SIZE_PTR == 8)
    // set values
    {
        SIMDType<8> obj;
        simd_set_all<int32>(obj, 456, 789);

        IS(obj.values_by_i32[0], 456);
        IS(obj.values_by_i32[1], 789);
        {
            int32 v0 = simd_get_one<0, int32>(obj);
            int32 v1 = simd_get_one<1, int32>(obj);
            IS(v0, 456);
            IS(v1, 789);
        }

        simd_set_all<int16>(obj, 789, 345, 222, 9009);

        IS(obj.values_by_i16[0], 789);
        IS(obj.values_by_i16[1], 345);
        IS(obj.values_by_i16[2], 222);
        IS(obj.values_by_i16[3], 9009);
        {
            int16 v0 = simd_get_one<0, int16>(obj);
            int16 v1 = simd_get_one<1, int16>(obj);
            int16 v2 = simd_get_one<2, int16>(obj);
            int16 v3 = simd_get_one<3, int16>(obj);
            IS(v0, 789);
            IS(v1, 345);
            IS(v2, 222);
            IS(v3, 9009);
        }

        simd_set_all<int8>(obj, 89, 55, 37, 24, 56, 35, 100, 31);
        IS(obj.values_by_i8[0], 89);
        IS(obj.values_by_i8[1], 55);
        IS(obj.values_by_i8[2], 37);
        IS(obj.values_by_i8[3], 24);
        IS(obj.values_by_i8[4], 56);
        IS(obj.values_by_i8[5], 35);
        IS(obj.values_by_i8[6], 100);
        IS(obj.values_by_i8[7], 31);
        {
            int8 v0 = simd_get_one<0, int8>(obj);
            int8 v1 = simd_get_one<1, int8>(obj);
            int8 v2 = simd_get_one<2, int8>(obj);
            int8 v3 = simd_get_one<3, int8>(obj);
            int8 v4 = simd_get_one<4, int8>(obj);
            int8 v5 = simd_get_one<5, int8>(obj);
            int8 v6 = simd_get_one<6, int8>(obj);
            int8 v7 = simd_get_one<7, int8>(obj);
            IS(v0, 89);
            IS(v1, 55);
            IS(v2, 37);
            IS(v3, 24);
            IS(v4, 56);
            IS(v5, 35);
            IS(v6, 100);
            IS(v7, 31);
        }
    }
#endif // 64bit MSVC
}
