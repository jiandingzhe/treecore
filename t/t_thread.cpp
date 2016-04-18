#include "treecore/TestFramework.h"
#include "treecore/Thread.h"

class MyThread : public treecore::Thread
{
public:
    MyThread(const treecore::String& name) : treecore::Thread(name)
    {

    }

    virtual ~MyThread() {}

    void run() override
    {
        printf("thread %s run\n", getThreadName().toRawUTF8());
    }
};

void TestFramework::content()
{
    MyThread* thread1 = new MyThread("");
    thread1->startThread();
    thread1->stopThread(-1);
    delete thread1;
}
