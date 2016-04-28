#include "treecore/OptionalScopedPointer.h"
#include "treecore/TestFramework.h"

using namespace treecore;

struct FooBar
{
    FooBar( bool& life_mark ): foo( 12 ), living( life_mark )
    {
        living = true;
    }

    ~FooBar()
    {
        if (!living)
        {
            treecore_assert_false;
            abort();
        }
        living = false;
    }

    int32 foo;
    bool& living;
};

typedef OptionalScopedPointer<FooBar> Ptr;

void TestFramework::content( int argc, char** argv )
{
    {
        bool obj_life = false;
        FooBar* obj   = new FooBar( obj_life );
        OK( obj_life );

        {
            Ptr p1( obj, false );
            IS( p1.get(), obj );
        }
        OK( obj_life );

        {
            Ptr p2( obj, true );
            IS( p2.get(), obj );
        }
        OK( !obj_life );
    }

    // move constructor
    {
        bool obj_life = false;
        FooBar* obj   = new FooBar( obj_life );
        {
            Ptr to_be_moved( obj, false );
            IS( to_be_moved.get(), obj );

            Ptr move_target( std::move( to_be_moved ) );
            OK( to_be_moved.get() == nullptr );
            IS( move_target.get(), obj );
            OK( obj_life );
        }
        OK( obj_life );

        {
            Ptr to_be_moved( obj, true );
            IS( to_be_moved.get(), obj );

            Ptr move_target( std::move( to_be_moved ) );
            OK( to_be_moved.get() == nullptr );
            IS( move_target.get(), obj );
            OK( obj_life );
        }
        OK( !obj_life );
    }

    // move assignment
    {
        bool obj_life = false;
        FooBar* obj   = new FooBar( obj_life );
        {
            Ptr to_be_moved( obj, false );
            IS( to_be_moved.get(), obj );

            Ptr move_target;
            OK( move_target.get() == nullptr );

            move_target = std::move( to_be_moved );
            OK( to_be_moved.get() == nullptr );
            IS( move_target.get(), obj );
            OK( obj_life );
        }
        OK( obj_life );

        {
            Ptr to_be_moved( obj, true );
            IS( to_be_moved.get(), obj );

            Ptr move_target;
            OK( move_target.get() == nullptr );

            move_target = std::move( to_be_moved );
            OK( to_be_moved.get() == nullptr );
            IS( move_target.get(), obj );
            OK( obj_life );
        }
        OK( !obj_life );
    }
}
