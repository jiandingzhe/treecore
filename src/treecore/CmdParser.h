#ifndef TREECORE_OPTION_PARSER
#define TREECORE_OPTION_PARSER

#include "treecore/Array.h"
#include "treecore/HashMap.h"
#include "treecore/RefCountHolder.h"
#include "treecore/RefCountObject.h"
#include "treecore/StringCast.h"

namespace treecore
{

class OptionParser;
class Option;

class ValueLimit
{
    friend class OptionParser;
    friend class Option;

    ValueLimit( uint16 min, uint16 max );

public:
    static ValueLimit Fixed( uint16 num );
    static ValueLimit Ranged( uint16 min, uint16 max );
    static ValueLimit Free();

protected:
    uint16 min;
    uint16 max;
};

class Option
{
    friend class OptionParser;

public:
    enum OptionFlag
    {
        FLAG_NONE = 0,
        FLAG_MULTI_KEY = 1,
    };

protected:
    struct ValueBase
    {
        ValueBase( ValueLimit limit );
        virtual ~ValueBase();
        virtual bool   cast_values( const Array<String>& values ) = 0;
        virtual String to_string() = 0;

        ValueLimit limit;
    };

    struct BoolSwitch: public ValueBase
    {
        BoolSwitch( bool* value );
        virtual ~BoolSwitch();

        virtual bool cast_values( const Array<String>& values );
        virtual String to_string();

        bool* value;
    };

    template<typename T>
    struct SingleValue: public ValueBase
    {
        SingleValue( T* value )
            : ValueBase( ValueLimit( 1, 1 ) )
            , value( value ) {}
        virtual ~SingleValue() {}

        virtual bool cast_values( const Array<String>& values )
        {
            if (values.size() != 1)
                abort();
            return fromString<T>( values[0], *value );
        }

        virtual String to_string()
        {
            return toString<T>( *value );
        }

        T* value;
    };

    template<typename T>
    struct MultiValue: public ValueBase
    {
        MultiValue( Array<T>* values, ValueLimit limit )
            : ValueBase( limit )
            , value_list( values ) {}

        virtual bool cast_values( const Array<String>& values )
        {
            for (size_t i = 0; i < values.size(); i++)
            {
                T tmp;
                if ( !fromString<T>( values[i], tmp ) )
                    return false;
                value_list->add( tmp );
            }
            return true;
        }

        virtual String to_string()
        {
            String result;
            for (size_t i = 0; i < value_list->size(); i++)
            {
                if (i > 0) result += " ";
                result += toString( (*value_list)[i] );
            }
            return result;
        }

        Array<T>* value_list;
    };

public:
    Option( const String& name_long,
            char          name_short,
            const String& group,
            bool*         value_store,
            int32         flags,
            const String& option_desc )
        : name_long( name_long )
        , name_short( name_short )
        , group( group )
        , store( new BoolSwitch( value_store ) )
        , flags( flags )
        , option_desc( option_desc )
        , value_desc( "" )
    {}

    template<typename T>
    Option( const String& name_long,
            char          name_short,
            const String& group,
            T*            value_store,
            int32         flags,
            const String& option_desc,
            const String& value_desc )
        : name_long( name_long )
        , name_short( name_short )
        , group( group )
        , store( new SingleValue<T>( value_store ) )
        , flags( flags )
        , option_desc( option_desc )
        , value_desc( value_desc )
    {}

    template<typename T>
    Option( const String& name_long,
            char          name_short,
            const String& group,
            Array<T>*     value_store,
            int32         flags,
            ValueLimit    value_num_limit,
            const String& option_desc,
            const String& value_desc )
        : name_long( name_long )
        , name_short( name_short )
        , group( group )
        , store( new MultiValue<T>( value_store, value_num_limit ) )
        , flags( flags )
        , option_desc( option_desc )
        , value_desc( value_desc )
    {}

    Option( Option&& other ) noexcept;

// copying is not allowed
    Option( const Option& other ) = delete;
    Option& operator = ( const Option& other ) = delete;

    virtual ~Option();

protected:
    static bool validate_name( char name );
    static bool validate_name( const String& name );
    String format_doc( size_t w_line, size_t w_key_left, size_t w_key, size_t w_key_right ) const;
    String format_doc_key() const;

protected:
    String name_long;
    char name_short;
    String group;
    ValueBase* store = nullptr;
    uint32 flags = 0;
    String option_desc;
    String value_desc;
};

class OptionParser: public RefCountObject
{
public:
    typedef RefCountHolder<OptionParser> Ptr;
    typedef RefCountHolder<const OptionParser> ConstPtr;

public:
    bool add_option( Option& option );
    void parse_options( int& argc, char const** argv );
    String format_document();

protected:
    Option* find_option_long( const String& key );
    Option* find_option_short( char key );

protected:
    size_t group_id_seed = 0;
    HashMap<String, size_t> group_ids;
    Array<Option*> options;
    HashMap<char, size_t> options_by_short;
    HashMap<String, size_t> options_by_long;
};

} // namespace treecore

#endif // TREECORE_OPTION_PARSER
