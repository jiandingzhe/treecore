#include "treecore/OptionParser.h"
#include "treecore/StringArray.h"
#include "treecore/StringCast.h"

#include <limits>
#include <cstdio>

#if TREECORE_OS_WINDOWS
#    include <windows.h>
#else
#    include <sys/ioctl.h>
#endif

namespace treecore
{

int32 get_terminal_width()
{
#if TREECORE_OS_WINDOWS
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &csbi );
    int32 columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return columns;
#else
    struct winsize win_size;
    ioctl( fileno( stdout ), TIOCGWINSZ, &win_size );
    return int32(win_size.ws_col);
#endif
}

String _dup_char_( int num_repeat, char content )
{
    String result;
    for (int i = 0; i < num_repeat; i++)
        result += content;
    return result;
}

void _break_desc_lines_( const String& input, size_t line_size, Array<String>& output )
{
    StringArray paragraphs;
    paragraphs.addTokens( input, "\n", "" );

    for (int line_i = 0; line_i < paragraphs.size(); line_i++)
    {
        output.add( String::empty() );
        const String& para = paragraphs[line_i];
        StringArray tokens;
        tokens.addTokens( para, " \t", "" );

        for (int i_token = 0; i_token < tokens.size(); i_token++)
        {
            const String& token = tokens[i_token];
            if (output.getLast().length() + token.length() + 1 > line_size)
                output.add( String::empty() );
            if (output.getLast().length() > 0)
                output.getLast() += " ";
            output.getLast() += token;
        }
    }
}

ValueLimit::ValueLimit( uint16 min, uint16 max )
    : min( min )
    , max( max ) {}

ValueLimit ValueLimit::Fixed( uint16 num )
{
    return ValueLimit( num, num );
}

ValueLimit ValueLimit::Ranged( uint16 min, uint16 max )
{
    return ValueLimit( min, max );
}

ValueLimit ValueLimit::Free()
{
    return ValueLimit( 0, std::numeric_limits<uint16>::max() );
}

Option::ValueBase::ValueBase( ValueLimit limit ): limit( limit ) {}

Option::ValueBase::~ValueBase() {}

Option::BoolSwitch::BoolSwitch( bool* value )
    : ValueBase( ValueLimit( 0, 0 ) )
    , value( value ) {}

Option::BoolSwitch::~BoolSwitch() {}

bool Option::BoolSwitch::cast_values( const Array<String>& values )
{
    treecore_assert( values.size() == 0 );
    *value = true;
    return true;
}

String Option::BoolSwitch::to_string()
{
    return String::empty();
}

Option::Option( Option&& other ) noexcept
    : name_long( other.name_long )
    , name_short( other.name_short )
    , group( other.group )
    , store( other.store )
    , flags( other.flags )
    , option_desc( other.option_desc )
    , value_desc( other.value_desc )
{
    other.store = nullptr;
}

Option::~Option()
{
    if (store)
        delete store;
}

String Option::format_doc( int32 w_line, int32 w_key_left, int32 w_key, int32 w_key_right ) const
{
    // validate key width
    String doc_key = format_doc_key();

    int w_key_remain = int(w_key) - doc_key.length();
    if (w_key_remain < 0)
    {
        fprintf( stderr, "option key exceeds width limit %lu: %d\n%s\n",
                 unsigned long(w_key), doc_key.length(), doc_key.toRawUTF8() );
        abort();
    }

    // get desc with default value
    String full_desc;
    String default_value_str = store->to_string();
    if ( default_value_str.length() )
        full_desc = "[" + default_value_str + "] " + option_desc;
    else
        full_desc = option_desc;

    // format document
    String result;
    if (w_key_left + w_key + w_key_right > w_line / 2)
    {
        // key is too wide, put desc in new line
        result += _dup_char_( w_key_left, ' ' ) + doc_key + "\n";

        Array<String> desc_lines;
        _break_desc_lines_( full_desc, w_line - 4, desc_lines );

        for (int i = 0; i < desc_lines.size(); i++)
            result += "    " + desc_lines[i] + "\n";
    }
    else
    {
        Array<String> desc_lines;
        _break_desc_lines_( full_desc, w_line - w_key_left - w_key - w_key_right, desc_lines );

        String leading_space = _dup_char_( w_key_left + w_key + w_key_right, ' ' );

        for (int i = 0; i < desc_lines.size(); i++)
        {
            if (i == 0)
            {
                result += _dup_char_( w_key_left, ' ' ) + doc_key + _dup_char_( w_key_remain, ' ' ) + _dup_char_( w_key_right, ' ' );
            }
            else
            {
                result += leading_space;
            }
            result += desc_lines[i] + "\n";
        }
    }

    return result;
}

String Option::format_doc_key() const
{
    String result;
    if (name_short)
    {
        result += char('-');
        result += name_short;
    }
    if ( name_long.length() )
    {
        if ( result.length() )
            result += "|";
        result += "--" + name_long;
    }
    if ( value_desc.length() )
    {
        if ( result.length() )
            result += " ";
        result += value_desc;
    }
    return result;
}

bool OptionParser::add_option( Option& option )
{
    bool long_inserted  = false;
    bool short_inserted = false;

    int32 idx = options.size();

    if (option.name_long.length() > 0)
    {
        HashMap<String, int32>::Iterator it( options_by_long );
        long_inserted = options_by_long.insertOrSelect( option.name_long, idx, it );
    }

    if (option.name_short != 0)
    {
        HashMap<char, int32>::Iterator it( options_by_short );
        short_inserted = options_by_short.insertOrSelect( option.name_short, idx, it );
    }

    if (long_inserted || short_inserted)
    {
        options.add( &option );

        const String& group = option.group;
        int32 id = group_id_seed++;

        group_ids.set( group, id );

        return true;
    }
    else
    {
        return false;
    }
}

struct IndexedValue
{
    int32  index;
    String value;
};

typedef Array<IndexedValue> IndexedValueGroup;
typedef HashMap<Option*, Array<IndexedValueGroup> > OptionValueMap;

int _groups_sum_( const Array<IndexedValueGroup>& groups )
{
    int re = 0;
    for (int i = 0; i < groups.size(); i++)
    {
        re += groups[i].size();
    }
    return re;
}

inline void _dump_value_group_( OptionValueMap& store, Option* option, IndexedValueGroup& values )
{
    store[option].add( values );
    values.clear();
}

void OptionParser::parse_options( int& argc, char const** argv )
{
    //
    // collect values by option
    //
    OptionValueMap values_by_option;
    {
        Option* curr_option = NULL;
        IndexedValueGroup curr_value_group;
        for (int i = 1; i < argc; i++)
        {
            String curr_token( argv[i] );

            // token is option key
            if (curr_token[0] == '-' && curr_token.length() >= 2)
            {
                // long key in "--xxx" style
                if (curr_token[1] == '-')
                {
                    Option* next_option = find_option_long( curr_token );

                    if (next_option)
                    {
                        // clear previous values
                        _dump_value_group_( values_by_option, curr_option, curr_value_group );
                        curr_option = next_option;
                    }
                    else
                    {
                        curr_value_group.add( IndexedValue{i, curr_token} );
                    }
                }

                // short key in "-x" style
                else
                {
                    Option* next_option = find_option_short( curr_token[1] );

                    if (next_option)
                    {
                        // clear previous values
                        _dump_value_group_( values_by_option, curr_option, curr_value_group );
                        curr_option = next_option;

                        // short key contains a value
                        if (curr_token.length() > 2)
                        {
                            String curr_token_value = curr_token.substring( 2 );

                            // extra check for that the option must accept value
                            if (curr_option->store->limit.max == 0)
                            {
                                fprintf( stderr, "ERROR: option -%c is bool switch, but a value \"%s\" is given via \"%s\"\n",
                                         curr_option->name_short, curr_token_value.toRawUTF8(), curr_token.toRawUTF8() );
                                exit( EXIT_FAILURE );
                            }

                            curr_value_group.add( IndexedValue{i, curr_token_value} );
                        }
                    }
                    else
                    {
                        curr_value_group.add( IndexedValue{i, curr_token} );
                    }
                }
            }
            // token is not key
            else
            {
                curr_value_group.add( IndexedValue{i, curr_token} );
            }
        } // argv cycle

        // lasting...
        _dump_value_group_( values_by_option, curr_option, curr_value_group );
    }

    //
    // set option values
    //
    Array<int> unused_indices;
    OptionValueMap::Iterator it( values_by_option );
    while ( it.next() )
    {
        Option* option = it.key();
        Array<IndexedValueGroup>& value_groups = it.value();

        if (option)
        {
            Array<String> values_plain;

            // a bool switch option, accept no values
            if (option->store->limit.max == 0)
            {
                // it should appear only once
                if (value_groups.size() > 1)
                {
                    fprintf( stderr, "ERROR: bool switch option \"%s\" appeared more than once\n",
                             option->format_doc_key().toRawUTF8() );
                    exit( EXIT_FAILURE );
                }

                // all values are unused
                for (int i_group = 0; i_group < value_groups.size(); i_group++)
                {
                    const IndexedValueGroup& value_group = value_groups[i_group];
                    for (int i_value = 0; i_value < value_group.size(); i_value++)
                    {
                        unused_indices.add( value_group[i_value].index );
                    }
                }
            }
            // accept one value
            else if (option->store->limit.min == 1 && option->store->limit.max == 1)
            {
                // it should appear only once
                if (value_groups.size() > 1)
                {
                    fprintf( stderr, "ERROR: single value option \"%s\" appeared more than once\n",
                             option->format_doc_key().toRawUTF8() );
                    exit( EXIT_FAILURE );
                }

                const IndexedValueGroup& first_group = value_groups[0];

                // we should have value
                if ( !first_group.size() )
                {
                    fprintf( stderr, "ERROR: single value option \"%s\" has no value\n",
                             option->format_doc_key().toRawUTF8() );
                    exit( EXIT_FAILURE );
                }

                // first value is used, all following values are unused
                for (int i_value = 0; i_value < first_group.size(); i_value++)
                {
                    if (i_value == 0)
                        values_plain.add( first_group[i_value].value );
                    else
                        unused_indices.add( first_group[i_value].index );
                }
            }
            // accept variable number of values
            else
            {
                const char* TEMPLATE_LOWER_THAN_MIN = 0;
                if (option->store->limit.min == option->store->limit.max)
                    TEMPLATE_LOWER_THAN_MIN = "ERROR: option \"%s\" requires %lu values, but only %lu got\n";
                else
                    TEMPLATE_LOWER_THAN_MIN = "ERROR: option \"%s\" requires %lu values at least, but only %lu got\n";

                // option can occur multiple times
                if (option->flags & Option::FLAG_MULTI_KEY)
                {
                    for (int i_group = 0; i_group < value_groups.size(); i_group++)
                    {
                        const IndexedValueGroup& value_group = value_groups[i_group];
                        for (int i_value = 0; i_value < value_group.size(); i_value++)
                        {
                            if (values_plain.size() < option->store->limit.max)
                                values_plain.add( value_group[i_value].value );
                            else
                                unused_indices.add( value_group[i_value].index );
                        }
                    }

                    // validate value number
                    if (values_plain.size() < option->store->limit.min)
                    {
                        fprintf( stderr, TEMPLATE_LOWER_THAN_MIN,
                                 option->format_doc_key().toRawUTF8(), option->store->limit.min, values_plain.size() );
                        exit( EXIT_FAILURE );
                    }
                }
                // option can occur only once
                else
                {
                    if (value_groups.size() > 1)
                    {
                        fprintf( stderr, "ERROR: multi value option \"%s\" appeared more than once\n",
                                 option->format_doc_key().toRawUTF8() );
                        exit( EXIT_FAILURE );
                    }

                    const IndexedValueGroup& first_group = value_groups[0];

                    // validate value number
                    if (first_group.size() < option->store->limit.min)
                    {

                        fprintf( stderr, TEMPLATE_LOWER_THAN_MIN,
                                 option->format_doc_key().toRawUTF8(), option->store->limit.min, first_group.size() );
                        exit( EXIT_FAILURE );
                    }

                    // store
                    for (int i_value = 0; i_value < first_group.size(); i_value++)
                    {
                        if (i_value < option->store->limit.max)
                            values_plain.add( first_group[i_value].value );
                        else
                            unused_indices.add( first_group[i_value].index );
                    }
                }
            }

            if ( !option->store->cast_values( values_plain ) )
            {
                fprintf( stderr, "ERROR: failed to cast values for option \"%s\"\n",
                         option->format_doc_key().toRawUTF8() );
                for (int i = 0; i < values_plain.size(); i++)
                {
                    fprintf( stderr, "ERROR:   \"%s\"\n",
                             values_plain[i].toRawUTF8() );
                }
                exit( EXIT_FAILURE );
            }
        }
        else
        {
            // ungrouped options, collect to unused
            for (int i_group = 0; i_group < value_groups.size(); i_group++)
            {
                const IndexedValueGroup& value_group = value_groups[i_group];
                for (int i_value = 0; i_value < value_group.size(); i_value++)
                {
                    unused_indices.add( value_group[i_value].index );
                }
            }
        }
    } // option cycle

    // modify argument
    // collect unprocessed names
    Array<const char*> buffer;
    for (int i = 0; i < unused_indices.size(); i++)
    {
        int index = unused_indices[i];
        if (index > 0)
            buffer.add( argv[index] );
    }

    // modify argv
    for (int i = 1; i < argc; i++)
    {
        if ( i <= buffer.size() )
        {
            argv[i] = buffer[i - 1];
        }
        else
        {
            argv[i] = NULL;
        }
    }

    argc = buffer.size() + 1;
}

Option* OptionParser::find_option_long( const String& key )
{
    String key_use = key.substring( 2 );

    // find exact key
    {
        HashMap<String, int32>::Iterator it( options_by_long );
        if ( options_by_long.select( key_use, it ) )
            return options[it.value()];
    }

    // find by prefix
    Array<String> matched_keys;
    Array<int32> matched_i;

    HashMap<String, int32>::Iterator it( options_by_long );

    while ( it.next() )
    {
        const String& curr_key = it.key();

        if ( curr_key.startsWith( key_use ) )
        {
            matched_keys.add( "--" + curr_key );
            matched_i.add( it.value() );
        }
    }

    if (matched_keys.size() == 0)
    {
        return nullptr;
    }
    else if (matched_keys.size() == 1)
    {
        return options[matched_i[0]];
    }
    else
    {
        fprintf( stderr, "ERROR: ambiguous option prefix \"%s\", which matches:\n", key.toRawUTF8() );
        for (int32 i = 0; i < matched_keys.size(); i++)
            fprintf( stderr, "ERROR:   %s\n", matched_keys[i].toRawUTF8() );
        exit( EXIT_FAILURE );
    }
}

Option* OptionParser::find_option_short( char key )
{
    HashMap<char, int32>::Iterator it( options_by_short );

    if ( options_by_short.select( key, it ) )
        return options[it.value()];
    else
    {
        return nullptr;
    }
}

struct GroupSort
{
    GroupSort( const HashMap<String, int32>& ids ): ids( ids ) {}

    int compareElements( const String& a, const String& b )
    {
        HashMap<String, int32>::ConstIterator it_a( ids );
        HashMap<String, int32>::ConstIterator it_b( ids );
        if ( !ids.select( a, it_a ) ) abort();
        if ( !ids.select( b, it_b ) ) abort();

        if ( it_a.value() < it_b.value() ) return -1;
        else if ( it_a.value() == it_b.value() ) return 0;
        else return 1;
    }

    const HashMap<String, int32>& ids;
};

String OptionParser::format_document()
{
    int32 line_width = get_terminal_width();

    // categorize by group
    HashMap<String, Array<Option*> > groups;
    for (int32 i = 0; i < options.size(); i++)
    {
        const String& group_name = options[i]->group;
        groups[group_name].add( options[i] );
    }

    String result;

    // get key length
    int32 max_doc_key_len = 0;

    for (int32 i = 0; i < options.size(); i++)
    {
        Option* opt    = options[i];
        String doc_key = opt->format_doc_key();
        if ( max_doc_key_len < doc_key.length() )
            max_doc_key_len = doc_key.length();
    }

    // sort groups by their occurance
    Array<String> group_names;
    {
        HashMap<String, int32>::ConstIterator it( group_ids );
        while ( it.next() )
            group_names.add( it.key() );

        GroupSort sorter( group_ids );
        group_names.sort( sorter );
    }

    // format document for each option group
    for (const String& group_name : group_names)
    {
        Array<Option*>& group_options = groups[group_name];

        if ( group_name.length() )
            result += group_name + "\n\n";

        for (int32 i = 0; i < group_options.size(); i++)
        {
            Option* opt = group_options[i];
            result += opt->format_doc( line_width, 2, max_doc_key_len, 2 );
            result += "\n";
        }
    }

    return result;
}

} // namespace treecore
