#include "treecore/CharPointer_UTF8.h"

namespace treecore
{

treecore_wchar CharPointer_UTF8::operator * () const noexcept
{
    const signed char byte = (signed char) *data;

    if (byte >= 0)
        return (treecore_wchar) (uint8) byte;

    uint32 n    = (uint32) (uint8) byte;
    uint32 mask = 0x7f;
    uint32 bit  = 0x40;
    size_t numExtraValues = 0;

    while ( (n & bit) != 0 && bit > 0x10 )
    {
        mask >>= 1;
        ++numExtraValues;
        bit >>= 1;
    }

    n &= mask;

    for (size_t i = 1; i <= numExtraValues; ++i)
    {
        const uint8 nextByte = (uint8) data [i];

        if ( (nextByte & 0xc0) != 0x80 )
            break;

        n <<= 6;
        n  |= (nextByte & 0x3f);
    }

    return (treecore_wchar) n;
}

size_t CharPointer_UTF8::length() const noexcept
{
    const CharType* d = data;
    size_t count = 0;

    for (;; )
    {
        const uint32 n = (uint32) (uint8) * d++;

        if ( (n & 0x80) != 0 )
        {
            while ( (*d & 0xc0) == 0x80 )
                ++d;
        }
        else if (n == 0)
            break;

        ++count;
    }

    return count;
}

treecore_wchar CharPointer_UTF8::getAndAdvance() noexcept
{
    const signed char byte = (signed char) *data++;

    if (byte >= 0)
        return (treecore_wchar) (uint8) byte;

    uint32 n    = (uint32) (uint8) byte;
    uint32 mask = 0x7f;
    uint32 bit  = 0x40;
    int numExtraValues = 0;

    while ( (n & bit) != 0 && bit > 0x8 )
    {
        mask >>= 1;
        ++numExtraValues;
        bit >>= 1;
    }

    n &= mask;

    while (--numExtraValues >= 0)
    {
        const uint32 nextByte = (uint32) (uint8) * data;

        if ( (nextByte & 0xc0) != 0x80 )
            break;

        ++data;
        n <<= 6;
        n  |= (nextByte & 0x3f);
    }

    return (treecore_wchar) n;
}

size_t CharPointer_UTF8::getBytesRequiredFor( const treecore_wchar charToWrite ) noexcept
{
    size_t num = 1;
    const uint32 c = (uint32) charToWrite;

    if (c >= 0x80)
    {
        ++num;
        if (c >= 0x800)
        {
            ++num;
            if (c >= 0x10000)
                ++num;
        }
    }

    return num;
}

void CharPointer_UTF8::write( const treecore_wchar charToWrite ) noexcept
{
    const uint32 c = (uint32) charToWrite;

    if (c >= 0x80)
    {
        int numExtraBytes = 1;
        if (c >= 0x800)
        {
            ++numExtraBytes;
            if (c >= 0x10000)
                ++numExtraBytes;
        }

        *data++ = (CharType) ( (uint32) ( 0xff << (7 - numExtraBytes) ) | ( c >> (numExtraBytes * 6) ) );

        while (--numExtraBytes >= 0)
            *data++ = (CharType) ( 0x80 | ( 0x3f & ( c >> (numExtraBytes * 6) ) ) );
    }
    else
    {
        *data++ = (CharType) c;
    }
}

bool CharPointer_UTF8::isValidString( const CharType* dataToTest, int maxBytesToRead )
{
    while (--maxBytesToRead >= 0 && *dataToTest != 0)
    {
        const signed char byte = (signed char) *dataToTest++;

        if (byte < 0)
        {
            uint8 bit = 0x40;
            int numExtraValues = 0;

            while ( (byte & bit) != 0 )
            {
                if (bit < 8)
                    return false;

                ++numExtraValues;
                bit >>= 1;

                if ( bit == 8 && (numExtraValues > maxBytesToRead
                                  || *CharPointer_UTF8( dataToTest - 1 ) > 0x10ffff) )
                    return false;
            }

            maxBytesToRead -= numExtraValues;
            if (maxBytesToRead < 0)
                return false;

            while (--numExtraValues >= 0)
                if ( (*dataToTest++ & 0xc0) != 0x80 )
                    return false;
        }
    }

    return true;
}

} // namespace treecore
