#include "treecore/TestFramework.h"

#include "treecore/WeakPtr.h"

class Foo
{
public:
    typedef treecore::WeakPtr<Foo> Ptr;

    Foo()
    {
        // If you're planning on using your WeakReferences in a multi-threaded situation, you may choose
        // to create a WeakReference to the object here in the constructor, which will pre-initialise the
        // embedded object, avoiding an (extremely unlikely) race condition that could occur if multiple
        // threads overlap while creating the first WeakReference to it.
    }

    ~Foo()
    {
        // This will zero all the references - you need to call this in your destructor.
        masterReference.clear();
    }

private:
    // You need to embed a variable of this type, with the name "masterReference" inside your object. If the
    // variable is not public, you should make your class a friend of WeakReference<MyObject> so that the
    // WeakReference class can access it.
    treecore::WeakPtr<Foo>::Master masterReference;
    friend class treecore::WeakPtr<Foo>;
};

void TestFramework::content()
{
    Foo* obj = new Foo();
    OK(obj);

    Foo::Ptr ref1 = obj;
    Foo::Ptr ref2 = obj;

    OK(ref1.get());
    OK(ref2.get());

    delete obj;

    OK(ref1.get() == nullptr);
    OK(ref2.get() == nullptr);

}
