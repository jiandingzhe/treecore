#include "treecore/HashMap.h"
#include "treecore/TestFramework.h"

struct TestValue
{
    TestValue() {}
    TestValue( int value ): content( value ) {}

    TestValue( const TestValue& other ) = delete;
    TestValue( TestValue&& other ): content( other.content )
    {
        other.content = 0;
    }

    TestValue& operator = ( const TestValue& other ) = delete;
    TestValue& operator = ( TestValue&& other )
    {
        content = other.content;
        other.content = 0;
        return *this;
    }

    int content = 456;
};

typedef treecore::HashMap<int, TestValue> MapType;

void TestFramework::content( int argc, char** argv )
{
    MapType map_tmp;

    MapType map_obj( std::move( map_tmp ) );
    OK( "map move constructor compiles" );

    {
        MapType::HashMapItem item_tmp;
        item_tmp.value.content = 789;
        MapType::HashMapItem item( std::move( item_tmp ) );
        OK( "map item move constructor compiles" );
    }

    {
        TestValue to_be_moved( 123 );
        MapType::Iterator i( map_obj );

        map_obj.insertOrSelect( 1, std::move( to_be_moved ), i );
        IS( to_be_moved.content, 0 );
        OK( map_obj.contains( 1 ) );

        OK( map_obj.select( 1, i ) );
        IS( i.value().content, 123 );
    }

    {
        TestValue to_be_moved( 1111111 );
        map_obj.set( 66666, std::move( to_be_moved ) );
        IS( to_be_moved.content, 0 );
        OK( map_obj.contains( 66666 ) );

        MapType::Iterator i( map_obj );
        OK( map_obj.select( 66666, i ) );
        IS( i.value().content, 1111111 );
    }

    {
        TestValue to_be_moved( 666 );
        OK( !map_obj.tryInsert( 1, std::move( to_be_moved ) ) );
        IS( map_obj.size(),      2 );
        IS( to_be_moved.content, 666 );

        OK( map_obj.tryInsert( 2, std::move( to_be_moved ) ) );
        IS( map_obj.size(),      3 );
        IS( map_obj[2].content,  666 );
        IS( to_be_moved.content, 0 );
    }
}
