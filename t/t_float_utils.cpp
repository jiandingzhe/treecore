#include "TestFramework.h"

#include "treecore/MPL.h"
#include "treecore/FloatUtils.h"
#include "treecore/IntTypes.h"



void TestFramework::content()
{
    bool int8_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::int8>::type, float>::value;
    bool int16_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::int16>::type, float>::value;
    bool int32_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::int32>::type, float>::value;
    bool int64_float_is_double = treecore::mpl_same_type<treecore::similar_float<treecore::int64>::type, double>::value;
    bool uint8_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::uint8>::type, float>::value;
    bool uint16_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::uint16>::type, float>::value;
    bool uint32_float_is_float = treecore::mpl_same_type<treecore::similar_float<treecore::uint32>::type, float>::value;
    bool uint64_float_is_double = treecore::mpl_same_type<treecore::similar_float<treecore::uint64>::type, double>::value;

    bool float_float_is_float = treecore::mpl_same_type<treecore::similar_float<float>::type, float>::value;
    bool double_double_is_double = treecore::mpl_same_type<treecore::similar_float<double>::type, double>::value;

    OK(int8_float_is_float);
    OK(int16_float_is_float);
    OK(int32_float_is_float);
    OK(int64_float_is_double);
    OK(uint8_float_is_float);
    OK(uint16_float_is_float);
    OK(uint32_float_is_float);
    OK(uint64_float_is_double);
    OK(float_float_is_float);
    OK(double_double_is_double);

    {
        float fvalue = 123.456f;
        float fvalue_inv;
        reinterpret_cast<treecore::uint32&>(fvalue_inv) = reinterpret_cast<treecore::uint32&>(fvalue) ^ treecore::float_sign_mask<float>::value;
        IS(fvalue_inv, -fvalue);
    }

    {
        float fvalue = -874.316f;
        float fvalue_inv;
        reinterpret_cast<treecore::uint32&>(fvalue_inv) = reinterpret_cast<treecore::uint32&>(fvalue) ^ treecore::float_sign_mask<float>::value;
        IS(fvalue_inv, -fvalue);
    }

    {
        double dvalue = 51234.56789;
        double dvalue_inv;
        reinterpret_cast<treecore::uint64&>(dvalue_inv) = reinterpret_cast<treecore::uint64&>(dvalue) ^ treecore::float_sign_mask<double>::value;
        IS(dvalue_inv, -dvalue);
    }

    {
        double dvalue = -97413.21315;
        double dvalue_inv;
        reinterpret_cast<treecore::uint64&>(dvalue_inv) = reinterpret_cast<treecore::uint64&>(dvalue) ^ treecore::float_sign_mask<double>::value;
        IS(dvalue_inv, -dvalue);
    }
}
