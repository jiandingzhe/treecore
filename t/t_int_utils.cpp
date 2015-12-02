#include "TestFramework.h"
#include "treecore/IntUtils.h"
#include "treecore/MPL.h"

void TestFramework::content()
{
    bool float_int_is_32 = treecore::mpl_same_type<typename treecore::similar_int<float>::type, treecore::int32>::value;
    bool float_uint_is_32 = treecore::mpl_same_type<typename treecore::similar_uint<float>::type, treecore::uint32>::value;

    bool double_int_is_64 = treecore::mpl_same_type<typename treecore::similar_int<double>::type, treecore::int64>::value;
    bool double_uint_is_64 = treecore::mpl_same_type<typename treecore::similar_uint<double>::type, treecore::uint64>::value;

    OK(float_int_is_32);
    OK(float_uint_is_32);
    OK(double_int_is_64);
    OK(double_uint_is_64);
}
