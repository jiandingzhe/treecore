#include "treecore/TestFramework.h"
#include "treecore/IntUtils.h"
#include "treecore/MPL.h"

void TestFramework::content()
{
    bool float_int_is_32 = treecore::mpl_same_type<typename treecore::similar_int<float>::type, treecore::int32>::value;
    bool float_uint_is_32 = treecore::mpl_same_type<typename treecore::similar_uint<float>::type, treecore::uint32>::value;

    bool double_int_is_64 = treecore::mpl_same_type<typename treecore::similar_int<double>::type, treecore::int64>::value;
    bool double_uint_is_64 = treecore::mpl_same_type<typename treecore::similar_uint<double>::type, treecore::uint64>::value;

    bool int8_larger_is_16 = treecore::mpl_same_type<typename treecore::larger_int<treecore::int8>::type, treecore::int16>::value;
    bool int16_larger_is_32 = treecore::mpl_same_type<typename treecore::larger_int<treecore::int16>::type, treecore::int32>::value;
    bool int32_larger_is_64 = treecore::mpl_same_type<typename treecore::larger_int<treecore::int32>::type, treecore::int64>::value;

    bool uint8_larger_is_16 = treecore::mpl_same_type<typename treecore::larger_uint<treecore::int8>::type, treecore::uint16>::value;
    bool uint16_larger_is_32 = treecore::mpl_same_type<typename treecore::larger_uint<treecore::int16>::type, treecore::uint32>::value;
    bool uint32_larger_is_64 = treecore::mpl_same_type<typename treecore::larger_uint<treecore::int32>::type, treecore::uint64>::value;

    OK(float_int_is_32);
    OK(float_uint_is_32);
    OK(double_int_is_64);
    OK(double_uint_is_64);

    OK(int8_larger_is_16);
    OK(int16_larger_is_32);
    OK(int32_larger_is_64);
    OK(uint8_larger_is_16);
    OK(uint16_larger_is_32);
    OK(uint32_larger_is_64);
}
