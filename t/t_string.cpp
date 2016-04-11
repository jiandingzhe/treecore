#include "treecore/MT19937.h"
#include "treecore/String.h"
#include "treecore/StringArray.h"
#include "treecore/StringRef.h"
#include "treecore/Variant.h"
#include "TestFramework.h"

using namespace treecore;

String createRandomWideCharString()
{
    treecore_wchar buffer[50] = { 0 };

    MT19937& rng = *MT19937::getInstance();

    for (int i = 0; i < numElementsInArray( buffer ) - 1; ++i)
    {
        if ( rng.next_bool() )
        {
            do
            {
                buffer[i] = treecore_wchar( 1 + rng.next_uint64_in_range( 0x10ffff - 1 ) );
            }
            while ( !CharPointer_UTF16::canRepresent( buffer[i] ) );
        }
        else
            buffer[i] = treecore_wchar( 1 + rng.next_uint64_in_range( 0xff ) );
    }

    return CharPointer_UTF32( buffer );
}

void TestFramework::content()
{
    MT19937& r = *MT19937::getInstance();
    {
        OK( "Compare" );
        IS( String().length(), 0 );
        IS( String(),     String::empty() );
        String s1, s2( "abcd" );
        OK( s1.isEmpty() && !s1.isNotEmpty() );
        OK( s2.isNotEmpty() && !s2.isEmpty() );
        IS( s2.length(),                    4 );

        s1 = "abcd";
        IS( s1,                            s2 );
        IS( s2,                            s1 );
        IS( s1,                        "abcd" );
        IS( s1,                       L"abcd" );
        IS( String( "abcd" ),          String( L"abcd" ) );
        IS( String( "abcdefg", 4 ),   L"abcd" );
        IS( String( "abcdefg", 4 ),    String( L"abcdefg", 4 ) );
        IS( String::charToString( 'x' ),  "x" );
        IS( String::charToString( 0 ), String::empty() );
        IS( s2 + "e",                 "abcde" );
        IS( s2 + 'e',                 "abcde" );
        IS( s2 + L'e',                "abcde" );
        IS( s2 + L"e",                "abcde" );
        OK( s1.equalsIgnoreCase( "abcD" ) );
        LT( s1, "abce" );
        GT( s1, "abbb" );
        OK( s1.startsWith( "ab" ) );
        OK( s1.startsWith( "abcd" ) );
        OK( !s1.startsWith( "abcde" ) );
        OK( s1.startsWithIgnoreCase( "aB" ) );
        OK( s1.endsWithIgnoreCase( "CD" ) );
        OK( s1.endsWith( "bcd" ) );
        OK( !s1.endsWith( "aabcd" ) );
        IS( s1.indexOf( String::empty() ),           0 );
        IS( s1.indexOfIgnoreCase( String::empty() ), 0 );
        OK( s1.startsWith( String::empty() ) );
        OK( s1.endsWith( String::empty() ) );
        OK( s1.contains( String::empty() ) );
        OK( s1.contains( "cd" ) );
        OK( s1.contains( "ab" ) );
        OK( s1.contains( "abcd" ) );
        OK( s1.containsChar( 'a' ) );
        OK( !s1.containsChar( 'x' ) );
        OK( !s1.containsChar( 0 ) );
        OK( String( "abc foo bar" ).containsWholeWord( "abc" ) );
        OK( String( "abc foo bar" ).containsWholeWord( "abc" ) );
    }

    {
        OK( "Operations" );

        String s( "012345678" );
        OK( s.hashCode() != 0 );
        OK( s.hashCode64() != 0 );
        OK( s.hashCode() != (s + s).hashCode() );
        OK( s.hashCode64() != (s + s).hashCode64() );
        IS( s.compare( String( "012345678" ) ), 0 );
        LT( s.compare( String( "012345679" ) ), 0 );
        GT( s.compare( String( "012345676" ) ), 0 );
        IS( String( "a" ).compareNatural( "A" ), 0 );
        LT( String( "A" ).compareNatural( "B" ), 0 );
        LT( String( "a" ).compareNatural( "B" ), 0 );
        GT( String( "10" ).compareNatural( "2" ),         0 );
        GT( String( "Abc 10" ).compareNatural( "aBC 2" ), 0 );
        LT( String( "Abc 1" ).compareNatural( "aBC 2" ), 0 );
        IS( s.substring( 2, 3 ),                                    String::charToString( s[2] ) );
        IS( s.substring( 0, 1 ),                                    String::charToString( s[0] ) );
        IS( s.getLastCharacter(),                                        s [s.length() - 1] );
        IS( String::charToString( s.getLastCharacter() ),                s.getLastCharacters( 1 ) );
        IS( s.substring( 0, 3 ),                                    L"012" );
        IS( s.substring( 0, 100 ),                                       s );
        IS( s.substring( -1, 100 ),                                      s );
        IS( s.substring( 3 ),                                     "345678" );
        IS( s.indexOf( String( L"45" ) ),                                4 );
        IS( String( "444445" ).indexOf( "45" ),                          4 );
        IS( String( "444445" ).lastIndexOfChar( '4' ),                   4 );
        IS( String( "45454545x" ).lastIndexOf( String( L"45" ) ),        6 );
        IS( String( "45454545x" ).lastIndexOfAnyOf( "456" ),             7 );
        IS( String( "45454545x" ).lastIndexOfAnyOf( String( L"456x" ) ), 8 );
        IS( String( "abABaBaBa" ).lastIndexOfIgnoreCase( "aB" ),         6 );
        IS( s.indexOfChar( L'4' ),                                       4 );
        IS( s + s,                                    "012345678012345678" );
        OK( s.startsWith( s ) );
        OK( s.startsWith( s.substring( 0, 4 ) ) );
        OK( s.startsWith( s.dropLastCharacters( 4 ) ) );
        OK( s.endsWith( s.substring( 5 ) ) );
        OK( s.endsWith( s ) );
        OK( s.contains( s.substring( 3, 6 ) ) );
        OK( s.contains( s.substring( 3 ) ) );
        OK( s.startsWithChar( s[0] ) );
        OK( s.endsWithChar( s.getLastCharacter() ) );
        IS( s [s.length()],                          0 );
        IS( String( "abcdEFGH" ).toLowerCase(), String( "abcdefgh" ) );
        IS( String( "abcdEFGH" ).toUpperCase(), String( "ABCDEFGH" ) );

        String s2( "123" );
        s2 << ( (int) 4 ) << ( (short) 5 ) << "678" << L"9" << '0';
        s2 += "xyz";
        IS( s2,       "1234567890xyz" );
        s2 += (int) 123;
        IS( s2,    "1234567890xyz123" );
        s2 += (int64) 123;
        IS( s2, "1234567890xyz123123" );

        OK( "Numeric conversions" );
        IS( String::empty().getIntValue(),                                    0 );
        IS( String::empty().getDoubleValue(),                               0.0 );
        IS( String::empty().getFloatValue(),                               0.0f );
        IS( s.getIntValue(),                                           12345678 );
        IS( s.getLargeIntValue(),                                         int64( 12345678 ) );
        IS( s.getDoubleValue(),                                      12345678.0 );
        IS( s.getFloatValue(),                                      12345678.0f );
        IS( String( -1234 ).getIntValue(),                                -1234 );
        IS( String( (int64) - 1234 ).getLargeIntValue(),                  -1234 );
        IS( String( -1234.56 ).getDoubleValue(),                              -1234.56 );
        IS( String( -1234.56f ).getFloatValue(),                              -1234.56f );
        IS( String( std::numeric_limits<int>::max() ).getIntValue(),        std::numeric_limits<int>::max() );
        IS( String( std::numeric_limits<int>::min() ).getIntValue(),        std::numeric_limits<int>::min() );
        IS( String( std::numeric_limits<int64>::max() ).getLargeIntValue(), std::numeric_limits<int64>::max() );
        IS( String( std::numeric_limits<int64>::min() ).getLargeIntValue(), std::numeric_limits<int64>::min() );
        IS( ("xyz" + s).getTrailingIntValue(),                                s.getIntValue() );
        IS( s.getHexValue32(),                                       0x12345678 );
        IS( s.getHexValue64(),                                            int64( 0x12345678 ) );
        OK( String::toHexString( 0x1234abcd ).equalsIgnoreCase( "1234abcd" ) );
        OK( String::toHexString( int64( 0x1234abcd ) ).equalsIgnoreCase( "1234abcd" ) );
        OK( String::toHexString( short(0x12ab) ).equalsIgnoreCase( "12ab" ) );

        unsigned char data[] = { 1, 2, 3, 4, 0xa, 0xb, 0xc, 0xd };
        OK( String::toHexString( data, 8, 0 ).equalsIgnoreCase( "010203040a0b0c0d" ) );
        OK( String::toHexString( data, 8, 1 ).equalsIgnoreCase( "01 02 03 04 0a 0b 0c 0d" ) );
        OK( String::toHexString( data, 8, 2 ).equalsIgnoreCase( "0102 0304 0a0b 0c0d" ) );

        OK( "Subsections" );
        String s3;
        s3 = "abcdeFGHIJ";
        OK( s3.equalsIgnoreCase( "ABCdeFGhiJ" ) );
        IS( s3.compareIgnoreCase( L"ABCdeFGhiJ" ), 0 );
        OK( s3.containsIgnoreCase( s3.substring( 3 ) ) );
        IS( s3.indexOfAnyOf( "xyzf", 2, true ),              5 );
        IS( s3.indexOfAnyOf( String( L"xyzf" ), 2, false ), -1 );
        IS( s3.indexOfAnyOf( "xyzF", 2, false ),             5 );
        OK( s3.containsAnyOf( String( L"zzzFs" ) ) );
        OK( s3.startsWith( "abcd" ) );
        OK( s3.startsWithIgnoreCase( String( L"abCD" ) ) );
        OK( s3.startsWith( String::empty() ) );
        OK( s3.startsWithChar( 'a' ) );
        OK( s3.endsWith( String( "HIJ" ) ) );
        OK( s3.endsWithIgnoreCase( String( L"Hij" ) ) );
        OK( s3.endsWith( String::empty() ) );
        OK( s3.endsWithChar( L'J' ) );
        IS( s3.indexOf( "HIJ" ),                        7 );
        IS( s3.indexOf( String( L"HIJK" ) ),           -1 );
        IS( s3.indexOfIgnoreCase( "hij" ),              7 );
        IS( s3.indexOfIgnoreCase( String( L"hijk" ) ), -1 );
        IS( s3.toStdString(),                          s3.toRawUTF8() );

        String s4( s3 );
        s4.append( String( "xyz123" ), 3 );
        IS( s4, s3 + "xyz" );

        LT( String( 1234 ), String( 1235 ) );
        GT( String( 1235 ), String( 1234 ) );
        GE( String( 1234 ), String( 1234 ) );
        LE( String( 1234 ), String( 1234 ) );
        GE( String( 1235 ), String( 1234 ) );
        LE( String( 1234 ), String( 1235 ) );

        String s5( "word word2 word3" );
        OK( s5.containsWholeWord( String( "word2" ) ) );
        IS( s5.indexOfWholeWord( "word2" ), 5 );
        OK( s5.containsWholeWord( String( L"word" ) ) );
        OK( s5.containsWholeWord( "word3" ) );
        OK( s5.containsWholeWord( s5 ) );
        OK( s5.containsWholeWordIgnoreCase( String( L"Word2" ) ) );
        IS( s5.indexOfWholeWordIgnoreCase( "Word2" ), 5 );
        OK( s5.containsWholeWordIgnoreCase( String( L"Word" ) ) );
        OK( s5.containsWholeWordIgnoreCase( "Word3" ) );
        OK( !s5.containsWholeWordIgnoreCase( String( L"Wordx" ) ) );
        OK( !s5.containsWholeWordIgnoreCase( "xWord2" ) );
        OK( s5.containsNonWhitespaceChars() );
        OK( s5.containsOnly( "ordw23 " ) );
        OK( !String( " \n\r\t" ).containsNonWhitespaceChars() );

        OK( s5.matchesWildcard( String( L"wor*" ), false ) );
        OK( s5.matchesWildcard( "wOr*", true ) );
        OK( s5.matchesWildcard( String( L"*word3" ), true ) );
        OK( s5.matchesWildcard( "*word?", true ) );
        OK( s5.matchesWildcard( String( L"Word*3" ), true ) );
        OK( !s5.matchesWildcard( String( L"*34" ), true ) );
        OK( String( "xx**y" ).matchesWildcard( "*y", true ) );
        OK( String( "xx**y" ).matchesWildcard( "x*y", true ) );
        OK( String( "xx**y" ).matchesWildcard( "xx*y", true ) );
        OK( String( "xx**y" ).matchesWildcard( "xx*", true ) );
        OK( String( "xx?y" ).matchesWildcard( "x??y", true ) );
        OK( String( "xx?y" ).matchesWildcard( "xx?y", true ) );
        OK( !String( "xx?y" ).matchesWildcard( "xx?y?", true ) );
        OK( String( "xx?y" ).matchesWildcard( "xx??", true ) );

        IS( s5.fromFirstOccurrenceOf( String::empty(), true, false ),    s5 );
        IS( s5.fromFirstOccurrenceOf( "xword2", true, false ),           s5.substring( 100 ) );
        IS( s5.fromFirstOccurrenceOf( String( L"word2" ), true, false ), s5.substring( 5 ) );
        IS( s5.fromFirstOccurrenceOf( "Word2", true, true ),             s5.substring( 5 ) );
        IS( s5.fromFirstOccurrenceOf( "word2", false, false ),           s5.getLastCharacters( 6 ) );
        IS( s5.fromFirstOccurrenceOf( "Word2", false, true ),            s5.getLastCharacters( 6 ) );

        IS( s5.fromLastOccurrenceOf( String::empty(), true, false ),     s5 );
        IS( s5.fromLastOccurrenceOf( "wordx", true, false ),             s5 );
        IS( s5.fromLastOccurrenceOf( "word", true, false ),              s5.getLastCharacters( 5 ) );
        IS( s5.fromLastOccurrenceOf( "worD", true, true ),               s5.getLastCharacters( 5 ) );
        IS( s5.fromLastOccurrenceOf( "word", false, false ),             s5.getLastCharacters( 1 ) );
        IS( s5.fromLastOccurrenceOf( "worD", false, true ),              s5.getLastCharacters( 1 ) );

        OK( s5.upToFirstOccurrenceOf( String::empty(), true, false ).isEmpty() );
        IS( s5.upToFirstOccurrenceOf( "word4", true, false ),                                 s5 );
        IS( s5.upToFirstOccurrenceOf( "word2", true, false ),                                 s5.substring( 0, 10 ) );
        IS( s5.upToFirstOccurrenceOf( "Word2", true, true ),                                  s5.substring( 0, 10 ) );
        IS( s5.upToFirstOccurrenceOf( "word2", false, false ),                                s5.substring( 0, 5 ) );
        IS( s5.upToFirstOccurrenceOf( "Word2", false, true ),                                 s5.substring( 0, 5 ) );

        IS( s5.upToLastOccurrenceOf( String::empty(), true, false ),                          s5 );
        IS( s5.upToLastOccurrenceOf( "zword", true, false ),                                  s5 );
        IS( s5.upToLastOccurrenceOf( "word", true, false ),                                   s5.dropLastCharacters( 1 ) );
        IS( s5.dropLastCharacters( 1 ).upToLastOccurrenceOf( "word", true, false ),           s5.dropLastCharacters( 1 ) );
        IS( s5.upToLastOccurrenceOf( "Word", true, true ),                                    s5.dropLastCharacters( 1 ) );
        IS( s5.upToLastOccurrenceOf( "word", false, false ),                                  s5.dropLastCharacters( 5 ) );
        IS( s5.upToLastOccurrenceOf( "Word", false, true ),                                   s5.dropLastCharacters( 5 ) );

        IS( s5.replace( "word", "xyz", false ),                                           String( "xyz xyz2 xyz3" ) );
        IS( s5.replace( "Word", "xyz", true ),                                   "xyz xyz2 xyz3" );
        IS( s5.dropLastCharacters( 1 ).replace( "Word", String( "xyz" ), true ), L"xyz xyz2 xyz" );
        IS( s5.replace( "Word", "", true ),                                               " 2 3" );
        IS( s5.replace( "Word2", "xyz", true ),                                           String( "word xyz word3" ) );
        OK( s5.replaceCharacter( L'w', 'x' ) != s5 );
        IS( s5.replaceCharacter( 'w', L'x' ).replaceCharacter( 'x', 'w' ), s5 );
        OK( s5.replaceCharacters( "wo", "xy" ) != s5 );
        IS( s5.replaceCharacters( "wo", "xy" ).replaceCharacters( "xy", "wo" ), s5 );
        IS( s5.retainCharacters( "1wordxya" ),                              String( "wordwordword" ) );
        OK( s5.retainCharacters( String::empty() ).isEmpty() );
        IS( s5.removeCharacters( "1wordxya" ),                        " 2 3" );
        IS( s5.removeCharacters( String::empty() ),                       s5 );
        IS( s5.initialSectionContainingOnly( "word" ),               L"word" );
        IS( String( "word" ).initialSectionContainingOnly( "word" ), L"word" );
        IS( s5.initialSectionNotContaining( String( "xyz " ) ),       String( "word" ) );
        IS( s5.initialSectionNotContaining( String( ";[:'/" ) ),          s5 );
        OK( !s5.isQuotedString() );
        OK( s5.quoted().isQuotedString() );
        OK( !s5.quoted().unquoted().isQuotedString() );
        OK( !String( "x'" ).isQuotedString() );
        OK( String( "'x" ).isQuotedString() );

        String s6( " \t xyz  \t\r\n" );
        IS( s6.trim(),                                  String( "xyz" ) );
        IS( s6.trim().trim(),                            "xyz" );
        IS( s5.trim(),                                      s5 );
        IS( s6.trimStart().trimEnd(),                       s6.trim() );
        IS( s6.trimStart().trimEnd(),                       s6.trimEnd().trimStart() );
        IS( s6.trimStart().trimStart().trimEnd().trimEnd(), s6.trimEnd().trimStart() );
        OK( s6.trimStart() != s6.trimEnd() );
        IS( ("\t\r\n " + s6 + "\t\n \r").trim(),          s6.trim() );
        IS( String::repeatedString( "xyz", 3 ), L"xyzxyzxyz" );
    }

    {

        OK( "UTF conversions" );

#define UTF_TEST( CharPointerType )                                                            \
    {                                                                                          \
        String s( createRandomWideCharString() );                                              \
                                                                                               \
        typename CharPointerType::CharType buffer [300];                                       \
                                                                                               \
        memset( buffer, 0xff, sizeof(buffer) );                                                \
        CharPointerType( buffer ).writeAll( s.toUTF32() );                                     \
        IS( String( CharPointerType( buffer ) ), s );                                          \
                                                                                               \
        memset( buffer, 0xff, sizeof(buffer) );                                                \
        CharPointerType( buffer ).writeAll( s.toUTF16() );                                     \
        IS( String( CharPointerType( buffer ) ), s );                                          \
                                                                                               \
        memset( buffer, 0xff, sizeof(buffer) );                                                \
        CharPointerType( buffer ).writeAll( s.toUTF8() );                                      \
        IS( String( CharPointerType( buffer ) ), s );                                          \
                                                                                               \
        OK( CharPointerType::isValidString( buffer, int( strlen( (const char*) buffer ) ) ) ); \
    }

        UTF_TEST( CharPointer_UTF32 );
        UTF_TEST( CharPointer_UTF16 );
        UTF_TEST( CharPointer_UTF8 );
#undef UTF_TEST
    }

    {
        OK( "StringArray" );

        StringArray s;
        s.addTokens( "4,3,2,1,0", ";,", "x" );
        IS( s.size(),                     5 );

        IS( s.joinIntoString( "-" ), String( "4-3-2-1-0" ) );
        s.remove( 2 );
        IS( s.joinIntoString( "--" ),            String( "4--3--1--0" ) );
        IS( s.joinIntoString( String::empty() ), String( "4310" ) );
        s.clear();
        IS( s.joinIntoString( "x" ), String::empty() );

        StringArray toks;
        toks.addTokens( "x,,", ";,", "" );
        IS( toks.size(),                     3 );
        IS( toks.joinIntoString( "-" ), String( "x--" ) );
        toks.clear();

        toks.addTokens( ",x,", ";,", "" );
        IS( toks.size(),                     3 );
        IS( toks.joinIntoString( "-" ), String( "-x-" ) );
        toks.clear();

        toks.addTokens( "x,'y,z',", ";,", "'" );
        IS( toks.size(),                     3 );
        IS( toks.joinIntoString( "-" ), String( "x-'y,z'-" ) );
    }

    {
        OK( "var" );

        var v1 = 0;
        var v2 = 0.16;
        var v3 = "0.16";
        var v4 = int64( 0 );
        var v5 = 0.0;
        OK( !v2.equals( v1 ) );
        OK( !v1.equals( v2 ) );
        OK( v2.equals( v3 ) );
        OK( !v3.equals( v1 ) );
        OK( !v1.equals( v3 ) );
        OK( v1.equals( v4 ) );
        OK( v4.equals( v1 ) );
        OK( v5.equals( v4 ) );
        OK( v4.equals( v5 ) );
        OK( !v2.equals( v4 ) );
        OK( !v4.equals( v2 ) );
    }

    MT19937::releaseInstance();
}
