#include "treecore/Queue.h"

#include "treecore/TestFramework.h"

void TestFramework::content()
{
    treecore::Queue<int> int_queue;
    OK(int_queue.isEmpty());

    int_queue.push(2);
    int_queue.push(3);
    int_queue.push(4);
    OK(!int_queue.isEmpty());

    int curr;
    OK(int_queue.pop(curr));
    IS(curr, 2);
    OK(int_queue.pop(curr));
    IS(curr, 3);

    int_queue.push(12345);
    OK(int_queue.pop(curr));
    IS(curr, 4);
    OK(int_queue.pop(curr));
    IS(curr, 12345);

    OK(!int_queue.pop(curr));
    OK(int_queue.isEmpty());

}
