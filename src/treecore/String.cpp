/*
   ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

   ==============================================================================
 */

#include "treecore/AtomicFunc.h"
#include "treecore/DebugUtils.h"
#include "treecore/ByteOrder.h"
#include "treecore/HeapBlock.h"
#include "treecore/NewLine.h"
#include "treecore/OutputStream.h"
#include "treecore/String.h"
#include "treecore/internal/String_private.h"

#include <cstring>
#include <cstdarg>
#include <locale>

namespace treecore {

#if TREECORE_COMPILER_MSVC
 #    pragma warning (push)
 #    pragma warning (disable: 4514 4996)
#endif

NewLine newLine;

static inline CharPointer_wchar_t castToCharPointer_wchar_t( const void* t ) noexcept
{
    return CharPointer_wchar_t( static_cast<const CharPointer_wchar_t::CharType*>(t) );
}

//==============================================================================
// (Mirrors the structure of StringHolder, but without the atomic member, so can be statically constructed)

const EmptyString& emptyString()
{
    static const EmptyString value = { 0x3fffffff, sizeof(String::CharPointerType::CharType), 0 };
    return value;
}

//==============================================================================
StringHolder::CharPointerType StringHolder::createUninitialisedBytes( size_t numBytes )
{
    numBytes = (numBytes + 3) & ~(size_t) 3;
    StringHolder* const s = reinterpret_cast<StringHolder*>(new char [sizeof(StringHolder) - sizeof(CharType) + numBytes]);
    atomic_store( &s->refCount, 0 );
    s->allocatedNumBytes = numBytes;
    return CharPointerType( s->text );
}

StringHolder::CharPointerType StringHolder::createFromCharPointer( const CharPointerType start, const CharPointerType end )
{
    if ( start.getAddress() == nullptr || start.isEmpty() )
        return CharPointerType( &(emptyString().text) );

    const size_t numBytes = (size_t) ( reinterpret_cast<const char*>( end.getAddress() )
                                       - reinterpret_cast<const char*>( start.getAddress() ) );
    const CharPointerType dest( createUninitialisedBytes( numBytes + sizeof(CharType) ) );
    std::memcpy( dest.getAddress(), start, numBytes );
    dest.getAddress()[numBytes / sizeof(CharType)] = 0;
    return dest;
}

StringHolder::CharPointerType StringHolder::createFromFixedLength( const char* const src, const size_t numChars )
{
    const CharPointerType dest( createUninitialisedBytes( numChars * sizeof(CharType) + sizeof(CharType) ) );
    CharPointerType( dest ).writeWithCharLimit( CharPointer_UTF8( src ), (int) (numChars + 1) );
    return dest;
}

void StringHolder::retain( const CharPointerType text ) noexcept
{
    StringHolder* const b = bufferFromText( text );

    if (b != (StringHolder*) &emptyString())
        ++(b->refCount);
}

StringHolder::CharPointerType StringHolder::makeUniqueWithByteSize( const CharPointerType text, size_t numBytes )
{
    StringHolder* const b = bufferFromText( text );

    if (b == (StringHolder*) &emptyString())
    {
        CharPointerType newText( createUninitialisedBytes( numBytes ) );
        newText.writeNull();
        return newText;
    }

    if (b->allocatedNumBytes >= numBytes && atomic_load( &b->refCount ) <= 0)
        return text;

    CharPointerType newText( createUninitialisedBytes( jmax( b->allocatedNumBytes, numBytes ) ) );
    std::memcpy( newText.getAddress(), text.getAddress(), b->allocatedNumBytes );
    release( b );

    return newText;
}

size_t StringHolder::getAllocatedNumBytes( const CharPointerType text ) noexcept
{
    return bufferFromText( text )->allocatedNumBytes;
}

const String& String::empty()
{
    static String empty_string_instance;
    return empty_string_instance;
}

//==============================================================================
String::String() noexcept: text( &(emptyString().text) )
{}

String::~String() noexcept
{
    StringHolder::release( text );
}

String::String ( const String& other ) noexcept: text( other.text )
{
    StringHolder::retain( text );
}

void String::swapWith( String& other ) noexcept
{
    std::swap( text, other.text );
}

void String::clear() noexcept
{
    StringHolder::release( text );
    text = &(emptyString().text);
}

String& String::operator = ( const String& other ) noexcept
{
    StringHolder::retain( other.text );
    StringHolder::release( text.atomicSwap( other.text ) );
    return *this;
}

String::String ( String&& other ) noexcept
    : text( other.text )
{
    other.text = &(emptyString().text); // why not use swap here?
}

String& String::operator = ( String&& other ) noexcept
{
    std::swap( text, other.text );
    return *this;
}

inline String::PreallocationBytes::PreallocationBytes ( const size_t num ) noexcept: numBytes( num ) {}

String::String ( const PreallocationBytes& preallocationSize )
    : text( StringHolder::createUninitialisedBytes( preallocationSize.numBytes + sizeof(CharPointerType::CharType) ) )
{}

void String::preallocateBytes( const size_t numBytesNeeded )
{
    text = StringHolder::makeUniqueWithByteSize( text, numBytesNeeded + sizeof(CharPointerType::CharType) );
}

int String::getReferenceCount() const noexcept
{
    return StringHolder::getReferenceCount( text );
}

//==============================================================================
String::String ( const char* const t )
    : text( StringHolder::createFromCharPointer( CharPointer_ASCII( t ) ) )
{
    /*  If you get an assertion here, then you're trying to create a string from 8-bit data
        that contains values greater than 127. These can NOT be correctly converted to unicode
        because there's no way for the String class to know what encoding was used to
        create them. The source data could be UTF-8, ASCII or one of many local code-pages.

        To get around this problem, you must be more explicit when you pass an ambiguous 8-bit
        string to the String class - so for example if your source data is actually UTF-8,
        you'd call String (CharPointer_UTF8 ("my utf8 string..")), and it would be able to
        correctly convert the multi-byte characters to unicode. It's *highly* recommended that
        you use UTF-8 with escape characters in your source code to represent extended characters,
        because there's no other way to represent these strings in a way that isn't dependent on
        the compiler, source code editor and platform.
     */
    treecore_assert( t == nullptr || CharPointer_ASCII::isValidString( t, std::numeric_limits<int>::max() ) );
}

String::String ( const char* const t, const size_t maxChars )
    : text( StringHolder::createFromCharPointer( CharPointer_ASCII( t ), maxChars ) )
{
    /*  If you get an assertion here, then you're trying to create a string from 8-bit data
        that contains values greater than 127. These can NOT be correctly converted to unicode
        because there's no way for the String class to know what encoding was used to
        create them. The source data could be UTF-8, ASCII or one of many local code-pages.

        To get around this problem, you must be more explicit when you pass an ambiguous 8-bit
        string to the String class - so for example if your source data is actually UTF-8,
        you'd call String (CharPointer_UTF8 ("my utf8 string..")), and it would be able to
        correctly convert the multi-byte characters to unicode. It's *highly* recommended that
        you use UTF-8 with escape characters in your source code to represent extended characters,
        because there's no other way to represent these strings in a way that isn't dependent on
        the compiler, source code editor and platform.
     */
    treecore_assert( t == nullptr || CharPointer_ASCII::isValidString( t, (int) maxChars ) );
}

String::String ( const wchar_t* const t ): text( StringHolder::createFromCharPointer( castToCharPointer_wchar_t( t ) ) ) {}
String::String ( const CharPointer_UTF8 t ): text( StringHolder::createFromCharPointer( t ) ) {}
String::String ( const CharPointer_UTF16 t ): text( StringHolder::createFromCharPointer( t ) ) {}
String::String ( const CharPointer_UTF32 t ): text( StringHolder::createFromCharPointer( t ) ) {}
String::String ( const CharPointer_ASCII t ): text( StringHolder::createFromCharPointer( t ) ) {}

String::String ( const CharPointer_UTF8 t, const size_t maxChars ): text( StringHolder::createFromCharPointer( t, maxChars ) ) {}
String::String ( const CharPointer_UTF16 t, const size_t maxChars ): text( StringHolder::createFromCharPointer( t, maxChars ) ) {}
String::String ( const CharPointer_UTF32 t, const size_t maxChars ): text( StringHolder::createFromCharPointer( t, maxChars ) ) {}
String::String ( const wchar_t* const t, size_t maxChars ): text( StringHolder::createFromCharPointer( castToCharPointer_wchar_t( t ), maxChars ) ) {}

String::String ( const CharPointer_UTF8 start, const CharPointer_UTF8 end ): text( StringHolder::createFromCharPointer( start, end ) ) {}
String::String ( const CharPointer_UTF16 start, const CharPointer_UTF16 end ): text( StringHolder::createFromCharPointer( start, end ) ) {}
String::String ( const CharPointer_UTF32 start, const CharPointer_UTF32 end ): text( StringHolder::createFromCharPointer( start, end ) ) {}

String::String ( const std::string& s ): text( StringHolder::createFromFixedLength( s.data(), s.size() ) ) {}

String String::charToString( const treecore_wchar character )
{
    String result( PreallocationBytes( CharPointerType::getBytesRequiredFor( character ) ) );
    CharPointerType t( result.text );
    t.write( character );
    t.writeNull();
    return result;
}

//==============================================================================
namespace NumberToStringConverters
{

char* numberToString( char* t, const int64 n ) noexcept
{
    if (n >= 0)
        return printDigits( t, static_cast<uint64>(n) );

    // NB: this needs to be careful not to call -std::numeric_limits<int64>::min(),
    // which has undefined behaviour
    t    = printDigits( t, static_cast<uint64>( -(n + 1) ) + 1 );
    *--t = '-';
    return t;
}

char* numberToString( char* t, uint64 v ) noexcept
{
    return printDigits( t, v );
}

char* numberToString( char* t, const int n ) noexcept
{
    if (n >= 0)
        return printDigits( t, static_cast<unsigned int>(n) );

    // NB: this needs to be careful not to call -std::numeric_limits<int>::min(),
    // which has undefined behaviour
    t    = printDigits( t, static_cast<unsigned int>( -(n + 1) ) + 1 );
    *--t = '-';
    return t;
}

char* numberToString( char* t, unsigned int v ) noexcept
{
    return printDigits( t, v );
}

StackArrayStream::StackArrayStream ( char* d )
{
    static const std::locale classicLocale( std::locale::classic() );
    imbue( classicLocale );
    setp( d, d + charsNeededForDouble );
}

size_t StackArrayStream::writeDouble( double n, int numDecPlaces )
{
    {
        std::ostream o( this );

        if (numDecPlaces > 0)
            o.precision( (std::streamsize) numDecPlaces );

        o << n;
    }

    return (size_t) ( pptr() - pbase() );
}

char* doubleToString( char* buffer, const int numChars, double n, int numDecPlaces, size_t& len ) noexcept
{
    if (numDecPlaces > 0 && numDecPlaces < 7 && n > -1.0e20 && n < 1.0e20)
    {
        char* const end = buffer + numChars;
        char* t = end;
        int64 v = (int64) (pow( 10.0, numDecPlaces ) * std::abs( n ) + 0.5);
        *--t = (char) 0;

        while (numDecPlaces >= 0 || v > 0)
        {
            if (numDecPlaces == 0)
                *--t = '.';

            *--t = (char) ( '0' + (v % 10) );

            v /= 10;
            --numDecPlaces;
        }

        if (n < 0)
            *--t = '-';

        len = (size_t) (end - t - 1);
        return t;
    }

    StackArrayStream strm( buffer );
    len = strm.writeDouble( n, numDecPlaces );
    treecore_assert( len <= charsNeededForDouble );
    return buffer;
}

String::CharPointerType createFromDouble( const double number, const int numberOfDecimalPlaces )
{
    char buffer [charsNeededForDouble];
    size_t len;
    char* const start = doubleToString( buffer, numElementsInArray( buffer ), (double) number, numberOfDecimalPlaces, len );
    return StringHolder::createFromFixedLength( start, len );
}

} // namespace NumberToStringConverters

//==============================================================================
String::String ( const int number ): text( NumberToStringConverters::createFromInteger( number ) ) {}
String::String ( const unsigned int number ): text( NumberToStringConverters::createFromInteger( number ) ) {}
String::String ( const short number ): text( NumberToStringConverters::createFromInteger( (int)number ) ) {}
String::String ( const unsigned short number ): text( NumberToStringConverters::createFromInteger( (unsigned int)number ) ) {}
String::String ( const int64 number ): text( NumberToStringConverters::createFromInteger( number ) ) {}
String::String ( const uint64 number ): text( NumberToStringConverters::createFromInteger( number ) ) {}

String::String ( const float number ): text( NumberToStringConverters::createFromDouble( (double)number, 0 ) ) {}
String::String ( const double number ): text( NumberToStringConverters::createFromDouble( number, 0 ) ) {}
String::String ( const float number, const int numberOfDecimalPlaces ): text( NumberToStringConverters::createFromDouble( (double)number, numberOfDecimalPlaces ) ) {}
String::String ( const double number, const int numberOfDecimalPlaces ): text( NumberToStringConverters::createFromDouble( number, numberOfDecimalPlaces ) ) {}

//==============================================================================
int String::length() const noexcept
{
    return (int) text.length();
}

static size_t findByteOffsetOfEnd( String::CharPointerType text ) noexcept
{
    return (size_t) ( ( (char*) text.findTerminatingNull().getAddress() ) - (char*) text.getAddress() );
}

size_t String::getByteOffsetOfEnd() const noexcept
{
    return findByteOffsetOfEnd( text );
}

treecore_wchar String::operator [] ( int index ) const noexcept
{
    treecore_assert( index == 0 || ( index > 0 && index <= (int) text.lengthUpTo( (size_t) index + 1 ) ) );
    return text [index];
}

template<typename Type>
struct HashGenerator
{
    template<typename CharPointer>
    static Type calculate( CharPointer t ) noexcept
    {
        Type result = Type();

        while ( !t.isEmpty() )
            result = ( (Type) multiplier ) * result + (Type) t.getAndAdvance();

        return result;
    }

    enum { multiplier = sizeof(Type) > 4 ? 101 : 31 };
};

int String::hashCode() const noexcept       { return HashGenerator<int>        ::calculate( text ); }
int64 String::hashCode64() const noexcept   { return HashGenerator<int64>      ::calculate( text ); }
std::size_t String::hash() const noexcept   { return HashGenerator<std::size_t>::calculate( text ); }

//==============================================================================
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const char* s2 ) noexcept              { return s1.compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const char* s2 ) noexcept              { return s1.compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const wchar_t* s2 ) noexcept           { return s1.compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const wchar_t* s2 ) noexcept           { return s1.compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, StringRef s2 ) noexcept                { return s1.getCharPointer().compare( s2.text ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, StringRef s2 ) noexcept                { return s1.getCharPointer().compare( s2.text ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const CharPointer_UTF8 s2 ) noexcept   { return s1.getCharPointer().compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const CharPointer_UTF8 s2 ) noexcept   { return s1.getCharPointer().compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const CharPointer_UTF16 s2 ) noexcept  { return s1.getCharPointer().compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const CharPointer_UTF16 s2 ) noexcept  { return s1.getCharPointer().compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator == ( const String& s1, const CharPointer_UTF32 s2 ) noexcept  { return s1.getCharPointer().compare( s2 ) == 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator != ( const String& s1, const CharPointer_UTF32 s2 ) noexcept  { return s1.getCharPointer().compare( s2 ) != 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator >  ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) > 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator <  ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) < 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator >= ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) >= 0; }
TREECORE_SHARED_API bool TREECORE_STDCALL operator <= ( const String& s1, const String& s2 ) noexcept            { return s1.compare( s2 ) <= 0; }

bool String::equalsIgnoreCase( const wchar_t* const t ) const noexcept
{
    return t != nullptr ? text.compareIgnoreCase( castToCharPointer_wchar_t( t ) ) == 0
           : isEmpty();
}

bool String::equalsIgnoreCase( const char* const t ) const noexcept
{
    return t != nullptr ? text.compareIgnoreCase( CharPointer_UTF8( t ) ) == 0
           : isEmpty();
}

bool String::equalsIgnoreCase( StringRef t ) const noexcept
{
    return text.compareIgnoreCase( t.text ) == 0;
}

bool String::equalsIgnoreCase( const String& other ) const noexcept
{
    return text == other.text
           || text.compareIgnoreCase( other.text ) == 0;
}

int String::compare( const String& other ) const noexcept           { return (text == other.text) ? 0 : text.compare( other.text ); }
int String::compare( const char* const other ) const noexcept       { return text.compare( CharPointer_UTF8( other ) ); }
int String::compare( const wchar_t* const other ) const noexcept    { return text.compare( castToCharPointer_wchar_t( other ) ); }
int String::compareIgnoreCase( const String& other ) const noexcept { return (text == other.text) ? 0 : text.compareIgnoreCase( other.text ); }

static int stringCompareRight( String::CharPointerType s1, String::CharPointerType s2 ) noexcept
{
    for (int bias = 0;; )
    {
        const treecore_wchar c1 = s1.getAndAdvance();
        const bool isDigit1 = CharacterFunctions::isDigit( c1 );

        const treecore_wchar c2 = s2.getAndAdvance();
        const bool isDigit2 = CharacterFunctions::isDigit( c2 );

        if ( !(isDigit1 || isDigit2) )   return bias;
        if (!isDigit1)                 return -1;
        if (!isDigit2)                 return 1;

        if (c1 != c2 && bias == 0)
            bias = c1 < c2 ? -1 : 1;

        treecore_assert( c1 != 0 && c2 != 0 );
    }
}

static int stringCompareLeft( String::CharPointerType s1, String::CharPointerType s2 ) noexcept
{
    for (;; )
    {
        const treecore_wchar c1 = s1.getAndAdvance();
        const bool isDigit1 = CharacterFunctions::isDigit( c1 );

        const treecore_wchar c2 = s2.getAndAdvance();
        const bool isDigit2 = CharacterFunctions::isDigit( c2 );

        if ( !(isDigit1 || isDigit2) )   return 0;
        if (!isDigit1)                 return -1;
        if (!isDigit2)                 return 1;
        if (c1 < c2)                    return -1;
        if (c1 > c2)                    return 1;
    }
}

static int naturalStringCompare( String::CharPointerType s1, String::CharPointerType s2 ) noexcept
{
    bool firstLoop = true;

    for (;; )
    {
        const bool hasSpace1 = s1.isWhitespace();
        const bool hasSpace2 = s2.isWhitespace();

        if ( (!firstLoop) && (hasSpace1 ^ hasSpace2) )
            return hasSpace2 ? 1 : -1;

        firstLoop = false;

        if (hasSpace1)  s1 = s1.findEndOfWhitespace();
        if (hasSpace2)  s2 = s2.findEndOfWhitespace();

        if ( s1.isDigit() && s2.isDigit() )
        {
            const int result = (*s1 == '0' || *s2 == '0') ? stringCompareLeft( s1, s2 )
                               : stringCompareRight( s1, s2 );

            if (result != 0)
                return result;
        }

        treecore_wchar c1 = s1.getAndAdvance();
        treecore_wchar c2 = s2.getAndAdvance();

        if (c1 != c2)
        {
            c1 = CharacterFunctions::toUpperCase( c1 );
            c2 = CharacterFunctions::toUpperCase( c2 );
        }

        if (c1 == c2)
        {
            if (c1 == 0)
                return 0;
        }
        else
        {
            const bool isAlphaNum1 = CharacterFunctions::isLetterOrDigit( c1 );
            const bool isAlphaNum2 = CharacterFunctions::isLetterOrDigit( c2 );

            if (isAlphaNum2 && !isAlphaNum1) return -1;
            if (isAlphaNum1 && !isAlphaNum2) return 1;

            return c1 < c2 ? -1 : 1;
        }

        treecore_assert( c1 != 0 && c2 != 0 );
    }
}

int String::compareNatural( StringRef other ) const noexcept
{
    return naturalStringCompare( getCharPointer(), other.text );
}

//==============================================================================
void String::append( const String& textToAppend, size_t maxCharsToTake )
{
    appendCharPointer( textToAppend.text, maxCharsToTake );
}

void String::appendCharPointer( const CharPointerType textToAppend )
{
    appendCharPointer( textToAppend, textToAppend.findTerminatingNull() );
}

void String::appendCharPointer( const CharPointerType startOfTextToAppend,
                                const CharPointerType endOfTextToAppend )
{
    treecore_assert( startOfTextToAppend.getAddress() != nullptr && endOfTextToAppend.getAddress() != nullptr );

    const int extraBytesNeeded = getAddressDifference( endOfTextToAppend.getAddress(),
                                                       startOfTextToAppend.getAddress() );
    treecore_assert( extraBytesNeeded >= 0 );

    if (extraBytesNeeded > 0)
    {
        const size_t byteOffsetOfNull = getByteOffsetOfEnd();
        preallocateBytes( byteOffsetOfNull + (size_t) extraBytesNeeded );

        CharPointerType::CharType* const newStringStart = addBytesToPointer( text.getAddress(), (int) byteOffsetOfNull );
        memcpy( newStringStart, startOfTextToAppend.getAddress(), (size_t) extraBytesNeeded );
        CharPointerType( addBytesToPointer( newStringStart, extraBytesNeeded ) ).writeNull();
    }
}

String& String::operator += ( const wchar_t* const t )
{
    appendCharPointer( castToCharPointer_wchar_t( t ) );
    return *this;
}

String& String::operator += ( const char* const t )
{
    appendCharPointer( CharPointer_UTF8( t ) ); // (using UTF8 here triggers a faster code-path than ascii)
    return *this;
}

String& String::operator += ( const String& other )
{
    if ( isEmpty() )
        return operator = ( other );

    appendCharPointer( other.text );
    return *this;
}

String& String::operator += ( const char ch )
{
    const char asString[] = { ch, 0 };
    return operator += ( asString );
}

String& String::operator += ( const wchar_t ch )
{
    const wchar_t asString[] = { ch, 0 };
    return operator += ( asString );
}

#if !TREECORE_NATIVE_WCHAR_IS_UTF32
String& String::operator += ( const treecore_wchar ch )
{
    const treecore_wchar asString[] = { ch, 0 };
    appendCharPointer( CharPointer_UTF32( asString ) );
    return *this;
}
#endif

String& String::operator += ( const int number )
{
    char buffer [16];
    char* end   = buffer + numElementsInArray( buffer );
    char* start = NumberToStringConverters::numberToString( end, number );
    appendCharPointer( CharPointerType( start ), CharPointerType( end ) );
    return *this;
}

String& String::operator += ( int64 number )
{
    char buffer [32];
    char* end   = buffer + numElementsInArray( buffer );
    char* start = NumberToStringConverters::numberToString( end, number );
	appendCharPointer(CharPointerType(start), CharPointerType(end));
    return *this;
}

//==============================================================================
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( const char* const s1, const String& s2 )    { String s( s1 ); return s += s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( const wchar_t* const s1, const String& s2 ) { String s( s1 ); return s += s2; }

TREECORE_SHARED_API String TREECORE_STDCALL operator + ( const char s1, const String& s2 )           { return String::charToString( (treecore_wchar) (uint8) s1 ) + s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( const wchar_t s1, const String& s2 )        { return String::charToString( s1 ) + s2; }

TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const String& s2 )               { return s1 += s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const char* const s2 )           { return s1 += s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const wchar_t* s2 )              { return s1 += s2; }

TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const char s2 )                  { return s1 += s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const wchar_t s2 )               { return s1 += s2; }

#if !TREECORE_NATIVE_WCHAR_IS_UTF32
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( const treecore_wchar s1, const String& s2 )     { return String::charToString( s1 ) + s2; }
TREECORE_SHARED_API String TREECORE_STDCALL operator + ( String s1, const treecore_wchar s2 )            { return s1 += s2; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const treecore_wchar s2 )         { return s1 += s2; }
#endif

TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const char s2 )               { return s1 += s2; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const wchar_t s2 )            { return s1 += s2; }

TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const char* const s2 )        { return s1 += s2; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const wchar_t* const s2 )     { return s1 += s2; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const String& s2 )            { return s1 += s2; }

TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const int number )            { return s1 += number; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const short number )          { return s1 += (int) number; }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const long number )           { return s1 += (int) number; }
//TREECORE_SHARED_API String& TREECORE_STDCALL operator<< (String& s1, const int64 number)          { return s1 += String (number); }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const float number )          { return s1 += String( number ); }
TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& s1, const double number )         { return s1 += String( number ); }
//TREECORE_SHARED_API String& TREECORE_STDCALL operator<< (String& s1, const uint64 number)         { return s1 += String (number); }

TREECORE_SHARED_API OutputStream& TREECORE_STDCALL operator << ( OutputStream& stream, const String& text )
{
    return operator << ( stream, StringRef( text ) );
}

TREECORE_SHARED_API OutputStream& TREECORE_STDCALL operator << ( OutputStream& stream, StringRef text )
{
    const size_t numBytes = CharPointer_UTF8::getBytesRequiredFor( text.text );
    stream.write( text.text.getAddress(), numBytes );

    return stream;
}

TREECORE_SHARED_API String& TREECORE_STDCALL operator << ( String& string1, const NewLine& )
{
    return string1 += NewLine::getDefault();
}

//==============================================================================
int String::indexOfChar( const treecore_wchar character ) const noexcept
{
    return text.indexOf( character );
}

int String::indexOfChar( const int startIndex, const treecore_wchar character ) const noexcept
{
    CharPointerType t( text );

    for (int i = 0; !t.isEmpty(); ++i)
    {
        if (i >= startIndex)
        {
            if (t.getAndAdvance() == character)
                return i;
        }
        else
        {
            ++t;
        }
    }

    return -1;
}

int String::lastIndexOfChar( const treecore_wchar character ) const noexcept
{
    CharPointerType t( text );
    int last = -1;

    for (int i = 0; !t.isEmpty(); ++i)
        if (t.getAndAdvance() == character)
            last = i;

    return last;
}

int String::indexOfAnyOf( StringRef charactersToLookFor, const int startIndex, const bool ignoreCase ) const noexcept
{
    CharPointerType t( text );

    for (int i = 0; !t.isEmpty(); ++i)
    {
        if (i >= startIndex)
        {
            if (charactersToLookFor.text.indexOf( t.getAndAdvance(), ignoreCase ) >= 0)
                return i;
        }
        else
        {
            ++t;
        }
    }

    return -1;
}

int String::indexOf( StringRef other ) const noexcept
{
    return other.isEmpty() ? 0 : text.indexOf( other.text );
}

int String::indexOfIgnoreCase( StringRef other ) const noexcept
{
    return other.isEmpty() ? 0 : CharacterFunctions::indexOfIgnoreCase( text, other.text );
}

int String::indexOf( const int startIndex, StringRef other ) const noexcept
{
    if ( other.isEmpty() )
        return -1;

    CharPointerType t( text );

    for (int i = startIndex; --i >= 0; )
    {
        if ( t.isEmpty() )
            return -1;

        ++t;
    }

    int found = t.indexOf( other.text );
    if (found >= 0)
        found += startIndex;
    return found;
}

int String::indexOfIgnoreCase( const int startIndex, StringRef other ) const noexcept
{
    if ( other.isEmpty() )
        return -1;

    CharPointerType t( text );

    for (int i = startIndex; --i >= 0; )
    {
        if ( t.isEmpty() )
            return -1;

        ++t;
    }

    int found = CharacterFunctions::indexOfIgnoreCase( t, other.text );
    if (found >= 0)
        found += startIndex;
    return found;
}

int String::lastIndexOf( StringRef other ) const noexcept
{
    if ( other.isNotEmpty() )
    {
        const int len = other.length();
        int i = length() - len;

        if (i >= 0)
        {
            for (CharPointerType n( text + i ); i >= 0; --i)
            {
                if (n.compareUpTo( other.text, len ) == 0)
                    return i;

                --n;
            }
        }
    }

    return -1;
}

int String::lastIndexOfIgnoreCase( StringRef other ) const noexcept
{
    if ( other.isNotEmpty() )
    {
        const int len = other.length();
        int i = length() - len;

        if (i >= 0)
        {
            for (CharPointerType n( text + i ); i >= 0; --i)
            {
                if (n.compareIgnoreCaseUpTo( other.text, len ) == 0)
                    return i;

                --n;
            }
        }
    }

    return -1;
}

int String::lastIndexOfAnyOf( StringRef charactersToLookFor, const bool ignoreCase ) const noexcept
{
    CharPointerType t( text );
    int last = -1;

    for (int i = 0; !t.isEmpty(); ++i)
        if (charactersToLookFor.text.indexOf( t.getAndAdvance(), ignoreCase ) >= 0)
            last = i;

    return last;
}

bool String::contains( StringRef other ) const noexcept
{
    return indexOf( other ) >= 0;
}

bool String::containsChar( const treecore_wchar character ) const noexcept
{
    return text.indexOf( character ) >= 0;
}

bool String::containsIgnoreCase( StringRef t ) const noexcept
{
    return indexOfIgnoreCase( t ) >= 0;
}

int String::indexOfWholeWord( StringRef word ) const noexcept
{
    if ( word.isNotEmpty() )
    {
        CharPointerType t( text );
        const int wordLen = word.length();
        const int end = (int) t.length() - wordLen;

        for (int i = 0; i <= end; ++i)
        {
            if ( t.compareUpTo( word.text, wordLen ) == 0
                 && ( i == 0 || !(t - 1).isLetterOrDigit() )
                 && !(t + wordLen).isLetterOrDigit() )
                return i;

            ++t;
        }
    }

    return -1;
}

int String::indexOfWholeWordIgnoreCase( StringRef word ) const noexcept
{
    if ( word.isNotEmpty() )
    {
        CharPointerType t( text );
        const int wordLen = word.length();
        const int end = (int) t.length() - wordLen;

        for (int i = 0; i <= end; ++i)
        {
            if ( t.compareIgnoreCaseUpTo( word.text, wordLen ) == 0
                 && ( i == 0 || !(t - 1).isLetterOrDigit() )
                 && !(t + wordLen).isLetterOrDigit() )
                return i;

            ++t;
        }
    }

    return -1;
}

bool String::containsWholeWord( StringRef wordToLookFor ) const noexcept
{
    return indexOfWholeWord( wordToLookFor ) >= 0;
}

bool String::containsWholeWordIgnoreCase( StringRef wordToLookFor ) const noexcept
{
    return indexOfWholeWordIgnoreCase( wordToLookFor ) >= 0;
}

//==============================================================================
template<typename CharPointer>
struct WildCardMatcher
{
    static bool matches( CharPointer wildcard, CharPointer test, const bool ignoreCase ) noexcept
    {
        for (;; )
        {
            const treecore_wchar wc = wildcard.getAndAdvance();

            if (wc == '*')
                return wildcard.isEmpty() || matchesAnywhere( wildcard, test, ignoreCase );

            if ( !characterMatches( wc, test.getAndAdvance(), ignoreCase ) )
                return false;

            if (wc == 0)
                return true;
        }
    }

    static bool characterMatches( const treecore_wchar wc, const treecore_wchar tc, const bool ignoreCase ) noexcept
    {
        return (wc == tc) || (wc == '?' && tc != 0)
               || ( ignoreCase && CharacterFunctions::toLowerCase( wc ) == CharacterFunctions::toLowerCase( tc ) );
    }

    static bool matchesAnywhere( const CharPointer wildcard, CharPointer test, const bool ignoreCase ) noexcept
    {
        for (; !test.isEmpty(); ++test)
            if ( matches( wildcard, test, ignoreCase ) )
                return true;

        return false;
    }
};

bool String::matchesWildcard( StringRef wildcard, const bool ignoreCase ) const noexcept
{
    return WildCardMatcher<CharPointerType>::matches( wildcard.text, text, ignoreCase );
}

//==============================================================================
String String::repeatedString( StringRef stringToRepeat, int numberOfTimesToRepeat )
{
    if (numberOfTimesToRepeat <= 0)
        return String();

    String result( PreallocationBytes( findByteOffsetOfEnd( stringToRepeat ) * (size_t) numberOfTimesToRepeat ) );
    CharPointerType n( result.text );

    while (--numberOfTimesToRepeat >= 0)
        n.writeAll( stringToRepeat.text );

    return result;
}

String String::paddedLeft( const treecore_wchar padCharacter, int minimumLength ) const
{
    treecore_assert( padCharacter != 0 );

    int extraChars = minimumLength;
    CharPointerType end( text );

    while ( !end.isEmpty() )
    {
        --extraChars;
        ++end;
    }

    if (extraChars <= 0 || padCharacter == 0)
        return *this;

    const size_t currentByteSize = (size_t) ( ( (char*) end.getAddress() ) - (char*) text.getAddress() );
    String result( PreallocationBytes( currentByteSize + (size_t) extraChars * CharPointerType::getBytesRequiredFor( padCharacter ) ) );
    CharPointerType n( result.text );

    while (--extraChars >= 0)
        n.write( padCharacter );

    n.writeAll( text );
    return result;
}

String String::paddedRight( const treecore_wchar padCharacter, int minimumLength ) const
{
    treecore_assert( padCharacter != 0 );

    int extraChars = minimumLength;
    CharPointerType end( text );

    while ( !end.isEmpty() )
    {
        --extraChars;
        ++end;
    }

    if (extraChars <= 0 || padCharacter == 0)
        return *this;

    const size_t currentByteSize = (size_t) ( ( (char*) end.getAddress() ) - (char*) text.getAddress() );
    String result( PreallocationBytes( currentByteSize + (size_t) extraChars * CharPointerType::getBytesRequiredFor( padCharacter ) ) );
    CharPointerType n( result.text );

    n.writeAll( text );

    while (--extraChars >= 0)
        n.write( padCharacter );

    n.writeNull();
    return result;
}

//==============================================================================
String String::replaceSection( int index, int numCharsToReplace, StringRef stringToInsert ) const
{
    if (index < 0)
    {
        // a negative index to replace from?
        treecore_assert_false;
        index = 0;
    }

    if (numCharsToReplace < 0)
    {
        // replacing a negative number of characters?
        numCharsToReplace = 0;
        treecore_assert_false;
    }

    CharPointerType insertPoint( text );

    for (int i = 0; i < index; ++i)
    {
        if ( insertPoint.isEmpty() )
        {
            // replacing beyond the end of the string?
            treecore_assert_false;
            return *this + stringToInsert;
        }

        ++insertPoint;
    }

    CharPointerType startOfRemainder( insertPoint );

    for (int i = 0; i < numCharsToReplace && !startOfRemainder.isEmpty(); ++i)
        ++startOfRemainder;

    if ( insertPoint == text && startOfRemainder.isEmpty() )
        return stringToInsert.text;

    const size_t initialBytes   = (size_t) ( ( (char*) insertPoint.getAddress() ) - (char*) text.getAddress() );
    const size_t newStringBytes = findByteOffsetOfEnd( stringToInsert );
    const size_t remainderBytes = (size_t) ( ( (char*) startOfRemainder.findTerminatingNull().getAddress() ) - (char*) startOfRemainder.getAddress() );

    const size_t newTotalBytes = initialBytes + newStringBytes + remainderBytes;
    if (newTotalBytes <= 0)
        return String();

    String result( PreallocationBytes( (size_t) newTotalBytes ) );

    char* dest = (char*) result.text.getAddress();
    memcpy( dest,             text.getAddress(),      initialBytes );
    dest += initialBytes;
    memcpy( dest,   stringToInsert.text.getAddress(), newStringBytes );
    dest += newStringBytes;
    memcpy( dest, startOfRemainder.getAddress(),      remainderBytes );
    dest += remainderBytes;
    CharPointerType( (CharPointerType::CharType*) dest ).writeNull();

    return result;
}

String String::replace( StringRef stringToReplace, StringRef stringToInsert, const bool ignoreCase ) const
{
    const int stringToReplaceLen = stringToReplace.length();
    const int stringToInsertLen  = stringToInsert.length();

    int i = 0;
    String result( *this );

    while ( ( i = ( ignoreCase ? result.indexOfIgnoreCase( i, stringToReplace )
                    : result.indexOf( i, stringToReplace ) ) ) >= 0 )
    {
        result = result.replaceSection( i, stringToReplaceLen, stringToInsert );
        i += stringToInsertLen;
    }

    return result;
}

class StringCreationHelper
{
public:
    StringCreationHelper ( const size_t initialBytes )
        : source( nullptr ), dest( nullptr ), allocatedBytes( initialBytes ), bytesWritten( 0 )
    {
        result.preallocateBytes( allocatedBytes );
        dest = result.getCharPointer();
    }

    StringCreationHelper ( const String::CharPointerType s )
        : source( s ), dest( nullptr ), allocatedBytes( StringHolder::getAllocatedNumBytes( s ) ), bytesWritten( 0 )
    {
        result.preallocateBytes( allocatedBytes );
        dest = result.getCharPointer();
    }

    void write( treecore_wchar c )
    {
        bytesWritten += String::CharPointerType::getBytesRequiredFor( c );

        if (bytesWritten > allocatedBytes)
        {
            allocatedBytes += jmax( (size_t) 8, allocatedBytes / 16 );
            const size_t destOffset = (size_t) ( ( (char*) dest.getAddress() ) - (char*) result.getCharPointer().getAddress() );
            result.preallocateBytes( allocatedBytes );
            dest = addBytesToPointer( result.getCharPointer().getAddress(), (int) destOffset );
        }

        dest.write( c );
    }

    String result;
    String::CharPointerType source;

private:
    String::CharPointerType dest;
    size_t allocatedBytes, bytesWritten;
};

String String::replaceCharacter( const treecore_wchar charToReplace, const treecore_wchar charToInsert ) const
{
    if ( !containsChar( charToReplace ) )
        return *this;

    StringCreationHelper builder( text );

    for (;; )
    {
        treecore_wchar c = builder.source.getAndAdvance();

        if (c == charToReplace)
            c = charToInsert;

        builder.write( c );

        if (c == 0)
            break;
    }

    return builder.result;
}

String String::replaceCharacters( StringRef charactersToReplace, StringRef charactersToInsertInstead ) const
{
    StringCreationHelper builder( text );

    for (;; )
    {
        treecore_wchar c = builder.source.getAndAdvance();

        const int index = charactersToReplace.text.indexOf( c );
        if (index >= 0)
            c = charactersToInsertInstead [index];

        builder.write( c );

        if (c == 0)
            break;
    }

    return builder.result;
}

//==============================================================================
bool String::startsWith( StringRef other ) const noexcept
{
    return text.compareUpTo( other.text, other.length() ) == 0;
}

bool String::startsWithIgnoreCase( StringRef other ) const noexcept
{
    return text.compareIgnoreCaseUpTo( other.text, other.length() ) == 0;
}

bool String::startsWithChar( const treecore_wchar character ) const noexcept
{
    treecore_assert( character != 0 ); // strings can't contain a null character!

    return *text == character;
}

bool String::endsWithChar( const treecore_wchar character ) const noexcept
{
    treecore_assert( character != 0 ); // strings can't contain a null character!

    if ( text.isEmpty() )
        return false;

    CharPointerType t( text.findTerminatingNull() );
    return *--t == character;
}

bool String::endsWith( StringRef other ) const noexcept
{
    CharPointerType end( text.findTerminatingNull() );
    CharPointerType otherEnd( other.text.findTerminatingNull() );

    while (end > text && otherEnd > other.text)
    {
        --end;
        --otherEnd;

        if (*end != *otherEnd)
            return false;
    }

    return otherEnd == other.text;
}

bool String::endsWithIgnoreCase( StringRef other ) const noexcept
{
    CharPointerType end( text.findTerminatingNull() );
    CharPointerType otherEnd( other.text.findTerminatingNull() );

    while (end > text && otherEnd > other.text)
    {
        --end;
        --otherEnd;

        if ( end.toLowerCase() != otherEnd.toLowerCase() )
            return false;
    }

    return otherEnd == other.text;
}

//==============================================================================
String String::toUpperCase() const
{
    StringCreationHelper builder( text );

    for (;; )
    {
        const treecore_wchar c = builder.source.toUpperCase();
        builder.write( c );

        if (c == 0)
            break;

        ++(builder.source);
    }

    return builder.result;
}

String String::toLowerCase() const
{
    StringCreationHelper builder( text );

    for (;; )
    {
        const treecore_wchar c = builder.source.toLowerCase();
        builder.write( c );

        if (c == 0)
            break;

        ++(builder.source);
    }

    return builder.result;
}

//==============================================================================
treecore_wchar String::getLastCharacter() const noexcept
{
    return isEmpty() ? treecore_wchar() : text [length() - 1];
}

String String::substring( int start, const int end ) const
{
    if (start < 0)
        start = 0;

    if (end <= start)
        return String();

    int i = 0;
    CharPointerType t1( text );

    while (i < start)
    {
        if ( t1.isEmpty() )
            return String();

        ++i;
        ++t1;
    }

    CharPointerType t2( t1 );
    while (i < end)
    {
        if ( t2.isEmpty() )
        {
            if (start == 0)
                return *this;

            break;
        }

        ++i;
        ++t2;
    }

    return String( t1, t2 );
}

String String::substring( int start ) const
{
    if (start <= 0)
        return *this;

    CharPointerType t( text );

    while (--start >= 0)
    {
        if ( t.isEmpty() )
            return String();

        ++t;
    }

    return String( t );
}

String String::dropLastCharacters( const int numberToDrop ) const
{
    return String( text, (size_t) jmax( 0, length() - numberToDrop ) );
}

String String::getLastCharacters( const int numCharacters ) const
{
    return String( text + jmax( 0, length() - jmax( 0, numCharacters ) ) );
}

String String::fromFirstOccurrenceOf( StringRef  sub,
                                      const bool includeSubString,
                                      const bool ignoreCase ) const
{
    const int i = ignoreCase ? indexOfIgnoreCase( sub )
                  : indexOf( sub );
    if (i < 0)
        return String();

    return substring( includeSubString ? i : i + sub.length() );
}

String String::fromLastOccurrenceOf( StringRef  sub,
                                     const bool includeSubString,
                                     const bool ignoreCase ) const
{
    const int i = ignoreCase ? lastIndexOfIgnoreCase( sub )
                  : lastIndexOf( sub );
    if (i < 0)
        return *this;

    return substring( includeSubString ? i : i + sub.length() );
}

String String::upToFirstOccurrenceOf( StringRef  sub,
                                      const bool includeSubString,
                                      const bool ignoreCase ) const
{
    const int i = ignoreCase ? indexOfIgnoreCase( sub )
                  : indexOf( sub );
    if (i < 0)
        return *this;

    return substring( 0, includeSubString ? i + sub.length() : i );
}

String String::upToLastOccurrenceOf( StringRef  sub,
                                     const bool includeSubString,
                                     const bool ignoreCase ) const
{
    const int i = ignoreCase ? lastIndexOfIgnoreCase( sub )
                  : lastIndexOf( sub );
    if (i < 0)
        return *this;

    return substring( 0, includeSubString ? i + sub.length() : i );
}

bool String::isQuotedString() const
{
    const String trimmed( trimStart() );

    return trimmed[0] == '"'
           || trimmed[0] == '\'';
}

String String::unquoted() const
{
    const int len = length();

    if (len == 0)
        return String();

    const treecore_wchar lastChar = text [len - 1];
    const int dropAtStart = (*text == '"' || *text == '\'') ? 1 : 0;
    const int dropAtEnd   = (lastChar == '"' || lastChar == '\'') ? 1 : 0;

    return substring( dropAtStart, len - dropAtEnd );
}

String String::quoted( const treecore_wchar quoteCharacter ) const
{
    if ( isEmpty() )
        return charToString( quoteCharacter ) + quoteCharacter;

    String t( *this );

    if ( !t.startsWithChar( quoteCharacter ) )
        t = charToString( quoteCharacter ) + t;

    if ( !t.endsWithChar( quoteCharacter ) )
        t += quoteCharacter;

    return t;
}

//==============================================================================
static String::CharPointerType findTrimmedEnd( const String::CharPointerType start,
                                               String::CharPointerType       end )
{
    while (end > start)
    {
        if ( !(--end).isWhitespace() )
        {
            ++end;
            break;
        }
    }

    return end;
}

String String::trim() const
{
    if ( isNotEmpty() )
    {
        CharPointerType start( text.findEndOfWhitespace() );

        const CharPointerType end( start.findTerminatingNull() );
        CharPointerType trimmedEnd( findTrimmedEnd( start, end ) );

        if (trimmedEnd <= start)
            return String();

        if (text < start || trimmedEnd < end)
            return String( start, trimmedEnd );
    }

    return *this;
}

String String::trimStart() const
{
    if ( isNotEmpty() )
    {
        const CharPointerType t( text.findEndOfWhitespace() );

        if (t != text)
            return String( t );
    }

    return *this;
}

String String::trimEnd() const
{
    if ( isNotEmpty() )
    {
        const CharPointerType end( text.findTerminatingNull() );
        CharPointerType trimmedEnd( findTrimmedEnd( text, end ) );

        if (trimmedEnd < end)
            return String( text, trimmedEnd );
    }

    return *this;
}

String String::trimCharactersAtStart( StringRef charactersToTrim ) const
{
    CharPointerType t( text );

    while (charactersToTrim.text.indexOf( *t ) >= 0)
        ++t;

    return t == text ? *this : String( t );
}

String String::trimCharactersAtEnd( StringRef charactersToTrim ) const
{
    if ( isNotEmpty() )
    {
        const CharPointerType end( text.findTerminatingNull() );
        CharPointerType trimmedEnd( end );

        while (trimmedEnd > text)
        {
            if (charactersToTrim.text.indexOf( *--trimmedEnd ) < 0)
            {
                ++trimmedEnd;
                break;
            }
        }

        if (trimmedEnd < end)
            return String( text, trimmedEnd );
    }

    return *this;
}

//==============================================================================
String String::retainCharacters( StringRef charactersToRetain ) const
{
    if ( isEmpty() )
        return String();

    StringCreationHelper builder( text );

    for (;; )
    {
        treecore_wchar c = builder.source.getAndAdvance();

        if (charactersToRetain.text.indexOf( c ) >= 0)
            builder.write( c );

        if (c == 0)
            break;
    }

    builder.write( 0 );
    return builder.result;
}

String String::removeCharacters( StringRef charactersToRemove ) const
{
    if ( isEmpty() )
        return String();

    StringCreationHelper builder( text );

    for (;; )
    {
        treecore_wchar c = builder.source.getAndAdvance();

        if (charactersToRemove.text.indexOf( c ) < 0)
            builder.write( c );

        if (c == 0)
            break;
    }

    return builder.result;
}

String String::initialSectionContainingOnly( StringRef permittedCharacters ) const
{
    for (CharPointerType t( text ); !t.isEmpty(); ++t)
        if (permittedCharacters.text.indexOf( *t ) < 0)
            return String( text, t );

    return *this;
}

String String::initialSectionNotContaining( StringRef charactersToStopAt ) const
{
    for (CharPointerType t( text ); !t.isEmpty(); ++t)
        if (charactersToStopAt.text.indexOf( *t ) >= 0)
            return String( text, t );

    return *this;
}

bool String::containsOnly( StringRef chars ) const noexcept
{
    for (CharPointerType t( text ); !t.isEmpty(); )
        if (chars.text.indexOf( t.getAndAdvance() ) < 0)
            return false;

    return true;
}

bool String::containsAnyOf( StringRef chars ) const noexcept
{
    for (CharPointerType t( text ); !t.isEmpty(); )
        if (chars.text.indexOf( t.getAndAdvance() ) >= 0)
            return true;

    return false;
}

bool String::containsNonWhitespaceChars() const noexcept
{
    for (CharPointerType t( text ); !t.isEmpty(); ++t)
        if ( !t.isWhitespace() )
            return true;

    return false;
}

// Note! The format parameter here MUST NOT be a reference, otherwise MS's va_start macro fails to work (but still compiles).
String String::formatted( const String pf, ... )
{
    size_t bufferSize = 256;

    for (;; )
    {
        va_list args;
        va_start( args, pf );

#if TREECORE_OS_WINDOWS
        HeapBlock<wchar_t> temp( bufferSize );
        const int num = (int) _vsnwprintf( temp.getData(), bufferSize - 1, pf.toWideCharPointer(), args );
#elif TREECORE_OS_ANDROID
        HeapBlock<char> temp( bufferSize );
        const int num = (int) vsnprintf( temp.getData(), bufferSize - 1, pf.toUTF8(), args );
#else
        HeapBlock<wchar_t> temp( bufferSize );
        const int num = (int) vswprintf( temp.getData(), bufferSize - 1, pf.toWideCharPointer(), args );
#endif

        va_end( args );

        if (num > 0)
            return String( temp );

        bufferSize += 256;

        if (num == 0 || bufferSize > 65536) // the upper limit is a sanity check to avoid situations where vprintf repeatedly
            break;                          // returns -1 because of an error rather than because it needs more space.
    }

    return String();
}

//==============================================================================
int String::getIntValue() const noexcept            { return text.getIntValue32(); }
int64 String::getLargeIntValue() const noexcept     { return text.getIntValue64(); }
float String::getFloatValue() const noexcept        { return (float) getDoubleValue(); }
double String::getDoubleValue() const noexcept      { return text.getDoubleValue(); }

int String::getTrailingIntValue() const noexcept
{
    int n    = 0;
    int mult = 1;
    CharPointerType t( text.findTerminatingNull() );

    while (--t >= text)
    {
        if ( !t.isDigit() )
        {
            if (*t == '-')
                n = -n;

            break;
        }

        n    += mult * (*t - '0');
        mult *= 10;
    }

    return n;
}

static const char hexDigits[] = "0123456789abcdef";

template<typename Type>
static String hexToString( Type v )
{
    String::CharPointerType::CharType buffer[32];
    String::CharPointerType::CharType* const end = buffer + numElementsInArray( buffer ) - 1;
    String::CharPointerType::CharType* t = end;
    *t = 0;

    do
    {
        *--t = hexDigits [(int) (v & 15)];
        v  >>= 4;

    } while (v != 0);

    return String( String::CharPointerType( t ),
                   String::CharPointerType( end ) );
}

String String::toHexString( int number )       { return hexToString( (unsigned int) number ); }
String String::toHexString( int64 number )     { return hexToString( (uint64) number ); }
String String::toHexString( short number )     { return toHexString( (int) (unsigned short) number ); }

String String::toHexString( const void* const d, const int size, const int groupSize )
{
    if (size <= 0)
        return String();

    int numChars = (size * 2) + 2;
    if (groupSize > 0)
        numChars += size / groupSize;

    String s( PreallocationBytes( sizeof(CharPointerType::CharType) * (size_t) numChars ) );

    const unsigned char* data = static_cast<const unsigned char*>(d);
    CharPointerType dest( s.text );

    for (int i = 0; i < size; ++i)
    {
        const unsigned char nextByte = *data++;
        dest.write( (treecore_wchar) hexDigits [nextByte >> 4] );
        dest.write( (treecore_wchar) hexDigits [nextByte & 0xf] );

        if ( groupSize > 0 && (i % groupSize) == (groupSize - 1) && i < (size - 1) )
            dest.write( (treecore_wchar) ' ' );
    }

    dest.writeNull();
    return s;
}

int String::getHexValue32() const noexcept    { return CharacterFunctions::HexParser<int>  ::parse( text ); }
int64 String::getHexValue64() const noexcept    { return CharacterFunctions::HexParser<int64>::parse( text ); }

//==============================================================================
String String::createStringFromData( const void* const unknownData, const int size )
{
    const uint8* const data = static_cast<const uint8*>(unknownData);

    if (size <= 0 || data == nullptr)
        return String();

    if (size == 1)
        return charToString( (treecore_wchar) data[0] );

    if ( CharPointer_UTF16::isByteOrderMarkBigEndian( data )
         || CharPointer_UTF16::isByteOrderMarkLittleEndian( data ) )
    {
        const int numChars = size / 2 - 1;

        StringCreationHelper builder( (size_t) numChars );

        const uint16* const src = (const uint16*) (data + 2);

        if ( CharPointer_UTF16::isByteOrderMarkBigEndian( data ) )
        {
            for (int i = 0; i < numChars; ++i)
                builder.write( (treecore_wchar) ByteOrder::swapIfLittleEndian( src[i] ) );
        }
        else
        {
            for (int i = 0; i < numChars; ++i)
                builder.write( (treecore_wchar) ByteOrder::swapIfBigEndian( src[i] ) );
        }

        builder.write( 0 );
        return builder.result;
    }

    const uint8* start = data;

    if ( size >= 3 && CharPointer_UTF8::isByteOrderMark( data ) )
        start += 3;

    return String( CharPointer_UTF8( (const char*) start ),
                   CharPointer_UTF8( (const char*) (data + size) ) );
}

//==============================================================================
static const treecore_wchar emptyChar = 0;

template<class CharPointerType_Src, class CharPointerType_Dest>
struct StringEncodingConverter
{
    static CharPointerType_Dest convert( const String& s )
    {
        String& source = const_cast<String&>(s);

        typedef typename CharPointerType_Dest::CharType DestChar;

        if ( source.isEmpty() )
            return CharPointerType_Dest( reinterpret_cast<const DestChar*>(&emptyChar) );

        CharPointerType_Src text( source.getCharPointer() );
        const size_t extraBytesNeeded = CharPointerType_Dest::getBytesRequiredFor( text ) + sizeof(typename CharPointerType_Dest::CharType);
        const size_t endOffset = (text.sizeInBytes() + 3) & ~3u; // the new string must be word-aligned or many Windows
                                                                 // functions will fail to read it correctly!
        source.preallocateBytes( endOffset + extraBytesNeeded );
        text = source.getCharPointer();

        void* const newSpace = addBytesToPointer( text.getAddress(), (int) endOffset );
        const CharPointerType_Dest extraSpace( static_cast<DestChar*>(newSpace) );

       #if TREECORE_DEBUG // (This just avoids spurious warnings from valgrind about the uninitialised bytes at the end of the buffer..)
        const size_t bytesToClear = (size_t) jmin( (int) extraBytesNeeded, 4 );
        zeromem( addBytesToPointer( newSpace, extraBytesNeeded - bytesToClear ), bytesToClear );
       #endif

        CharPointerType_Dest( extraSpace ).writeAll( text );
        return extraSpace;
    }
};

template<>
struct StringEncodingConverter<CharPointer_UTF8, CharPointer_UTF8>
{
    static CharPointer_UTF8 convert( const String& source ) noexcept   { return CharPointer_UTF8( (CharPointer_UTF8::CharType*) source.getCharPointer().getAddress() ); }
};

template<>
struct StringEncodingConverter<CharPointer_UTF16, CharPointer_UTF16>
{
    static CharPointer_UTF16 convert( const String& source ) noexcept  { return CharPointer_UTF16( (CharPointer_UTF16::CharType*) source.getCharPointer().getAddress() ); }
};

template<>
struct StringEncodingConverter<CharPointer_UTF32, CharPointer_UTF32>
{
    static CharPointer_UTF32 convert( const String& source ) noexcept  { return CharPointer_UTF32( (CharPointer_UTF32::CharType*) source.getCharPointer().getAddress() ); }
};

CharPointer_UTF8 String::toUTF8()  const { return StringEncodingConverter<CharPointerType, CharPointer_UTF8 >::convert( *this ); }
CharPointer_UTF16 String::toUTF16() const { return StringEncodingConverter<CharPointerType, CharPointer_UTF16>::convert( *this ); }
CharPointer_UTF32 String::toUTF32() const { return StringEncodingConverter<CharPointerType, CharPointer_UTF32>::convert( *this ); }

const char* String::toRawUTF8() const
{
    return toUTF8().getAddress();
}

const wchar_t* String::toWideCharPointer() const
{
    static_assert( sizeof(CharPointer_wchar_t::CharType) == sizeof(wchar_t), "" );
    return (wchar_t*) StringEncodingConverter<CharPointerType, CharPointer_wchar_t>::convert( *this ).getAddress();
}

std::string String::toStdString() const
{
    return std::string( toRawUTF8() );
}

//==============================================================================
template<class CharPointerType_Src, class CharPointerType_Dest>
struct StringCopier
{
    static size_t copyToBuffer( const CharPointerType_Src source, typename CharPointerType_Dest::CharType* const buffer, const size_t maxBufferSizeBytes )
    {
        treecore_assert( ( (ssize_t) maxBufferSizeBytes ) >= 0 ); // keep this value positive!

        if (buffer == nullptr)
            return CharPointerType_Dest::getBytesRequiredFor( source ) + sizeof(typename CharPointerType_Dest::CharType);

        return CharPointerType_Dest( buffer ).writeWithDestByteLimit( source, maxBufferSizeBytes );
    }
};

size_t String::copyToUTF8( CharPointer_UTF8::CharType* const buffer, size_t maxBufferSizeBytes ) const noexcept
{
    return StringCopier<CharPointerType, CharPointer_UTF8>::copyToBuffer( text, buffer, maxBufferSizeBytes );
}

size_t String::copyToUTF16( CharPointer_UTF16::CharType* const buffer, size_t maxBufferSizeBytes ) const noexcept
{
    return StringCopier<CharPointerType, CharPointer_UTF16>::copyToBuffer( text, buffer, maxBufferSizeBytes );
}

size_t String::copyToUTF32( CharPointer_UTF32::CharType* const buffer, size_t maxBufferSizeBytes ) const noexcept
{
    return StringCopier<CharPointerType, CharPointer_UTF32>::copyToBuffer( text, buffer, maxBufferSizeBytes );
}

//==============================================================================
size_t String::getNumBytesAsUTF8() const noexcept
{
    return CharPointer_UTF8::getBytesRequiredFor( text );
}

String String::fromUTF8( const char* const buffer, int bufferSizeBytes )
{
    if (buffer != nullptr)
    {
        if (bufferSizeBytes < 0)
            return String( CharPointer_UTF8( buffer ) );

        if (bufferSizeBytes > 0)
        {
            treecore_assert( CharPointer_UTF8::isValidString( buffer, bufferSizeBytes ) );
            return String( CharPointer_UTF8( buffer ), CharPointer_UTF8( buffer + bufferSizeBytes ) );
        }
    }

    return String();
}

#if TREECORE_COMPILER_MSVC
#    pragma warning (pop)
#endif

//==============================================================================
StringRef::StringRef() noexcept: text( (const String::CharPointerType::CharType*) "\0\0\0" )
{}

StringRef::StringRef ( const char* stringLiteral ) noexcept
    : text( stringLiteral )
{
    treecore_assert( stringLiteral != nullptr ); // This must be a valid string literal, not a null pointer!!

#if TREECORE_NATIVE_WCHAR_IS_UTF8
    /*  If you get an assertion here, then you're trying to create a string from 8-bit data
        that contains values greater than 127. These can NOT be correctly converted to unicode
        because there's no way for the String class to know what encoding was used to
        create them. The source data could be UTF-8, ASCII or one of many local code-pages.

        To get around this problem, you must be more explicit when you pass an ambiguous 8-bit
        string to the StringRef class - so for example if your source data is actually UTF-8,
        you'd call StringRef (CharPointer_UTF8 ("my utf8 string..")), and it would be able to
        correctly convert the multi-byte characters to unicode. It's *highly* recommended that
        you use UTF-8 with escape characters in your source code to represent extended characters,
        because there's no other way to represent these strings in a way that isn't dependent on
        the compiler, source code editor and platform.
     */
    treecore_assert( CharPointer_ASCII::isValidString( stringLiteral, std::numeric_limits<int>::max() ) );
#endif
}

StringRef::StringRef ( String::CharPointerType stringLiteral ) noexcept: text( stringLiteral )
{
    treecore_assert( stringLiteral.getAddress() != nullptr ); // This must be a valid string literal, not a null pointer!!
}

StringRef::StringRef ( const String& string ) noexcept: text( string.getCharPointer() ) {}

}
