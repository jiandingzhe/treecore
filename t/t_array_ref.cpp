#include "TestFramework.h"
#include "treecore/ArrayRef.h"

using namespace treecore;

void TestFramework::content()
{
    Array<int> array_obj;
    array_obj.add(123);
    array_obj.add(456);
    array_obj.add(789);

    {

        ArrayRef<const int> ref(array_obj);
        IS(ref.size(), 3);
        IS(ref[0], 123);
        IS(ref[1], 456);
        IS(ref[2], 789);
    }

    {
        const Array<int>& array_obj_const(array_obj);
        ArrayRef<const int> ref(array_obj_const);
        IS(ref.size(), 3);
        IS(ref[0], 123);
        IS(ref[1], 456);
        IS(ref[2], 789);
    }

    int data[3] = {987, 654, 321};

    {
        ArrayRef<const int> ref(data, 3);
        IS(ref.size(), 3);
        IS(ref[0], 987);
        IS(ref[1], 654);
        IS(ref[2], 321);
    }

    {
        const int (&dataref)[3](data);
        ArrayRef<const int> ref(dataref, 3);
        IS(ref.size(), 3);
        IS(ref[0], 987);
        IS(ref[1], 654);
        IS(ref[2], 321);
    }
}
