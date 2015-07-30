#ifndef TREECORE_STRING_PRIVATE_H
#define TREECORE_STRING_PRIVATE_H

#include "treecore/Config.h"
#include "treecore/String.h"

namespace treecore
{

struct EmptyString
{
    int refCount;
    size_t allocatedBytes;
    String::CharPointerType::CharType text;
};

extern const EmptyString emptyString;

class StringHolder
{
public:
    StringHolder() = delete;

    typedef String::CharPointerType CharPointerType;
    typedef String::CharPointerType::CharType CharType;

    //==============================================================================
    static CharPointerType createUninitialisedBytes (size_t numBytes);

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer text)
    {
        if (text.getAddress() == nullptr || text.isEmpty())
            return CharPointerType (&(emptyString.text));

        CharPointer t (text);
        size_t bytesNeeded = sizeof (CharType);

        while (! t.isEmpty())
            bytesNeeded += CharPointerType::getBytesRequiredFor (t.getAndAdvance());

        const CharPointerType dest (createUninitialisedBytes (bytesNeeded));
        CharPointerType (dest).writeAll (text);
        return dest;
    }

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer text, size_t maxChars)
    {
        if (text.getAddress() == nullptr || text.isEmpty() || maxChars == 0)
            return CharPointerType (&(emptyString.text));

        CharPointer end (text);
        size_t numChars = 0;
        size_t bytesNeeded = sizeof (CharType);

        while (numChars < maxChars && ! end.isEmpty())
        {
            bytesNeeded += CharPointerType::getBytesRequiredFor (end.getAndAdvance());
            ++numChars;
        }

        const CharPointerType dest (createUninitialisedBytes (bytesNeeded));
        CharPointerType (dest).writeWithCharLimit (text, (int) numChars + 1);
        return dest;
    }

    template <class CharPointer>
    static CharPointerType createFromCharPointer (const CharPointer start, const CharPointer end)
    {
        if (start.getAddress() == nullptr || start.isEmpty())
            return CharPointerType (&(emptyString.text));

        CharPointer e (start);
        int numChars = 0;
        size_t bytesNeeded = sizeof (CharType);

        while (e < end && ! e.isEmpty())
        {
            bytesNeeded += CharPointerType::getBytesRequiredFor (e.getAndAdvance());
            ++numChars;
        }

        const CharPointerType dest (createUninitialisedBytes (bytesNeeded));
        CharPointerType (dest).writeWithCharLimit (start, numChars + 1);
        return dest;
    }

    static CharPointerType createFromCharPointer (const CharPointerType start, const CharPointerType end);

    static CharPointerType createFromFixedLength (const char* const src, const size_t numChars);

    //==============================================================================
    static void retain (const CharPointerType text) NOEXCEPT;

    static inline void release (StringHolder* const b) NOEXCEPT
    {
        if (b != (StringHolder*) &emptyString)
            if (--(b->refCount) == -1)
                delete[] reinterpret_cast<char*> (b);
    }

    static void release (const CharPointerType text) NOEXCEPT
    {
        release(bufferFromText (text));
    }

    static inline int getReferenceCount (const CharPointerType text) NOEXCEPT
    {
        return atomic_load(&bufferFromText(text)->refCount) + 1;
    }

    //==============================================================================
    static CharPointerType makeUniqueWithByteSize (const CharPointerType text, size_t numBytes);

    static size_t getAllocatedNumBytes (const CharPointerType text) NOEXCEPT;

    //==============================================================================
    int refCount;
    size_t allocatedNumBytes;
    CharType text[1];

private:
    static inline StringHolder* bufferFromText (const CharPointerType text) NOEXCEPT
    {
        // (Can't use offsetof() here because of warnings about this not being a POD)
        return reinterpret_cast<StringHolder*> (reinterpret_cast<char*> (text.getAddress())
                    - (reinterpret_cast<size_t> (reinterpret_cast<StringHolder*> (1)->text) - 1));
    }

    void compileTimeChecks()
    {
        // Let me know if any of these assertions fail on your system!
       #if JUCE_NATIVE_WCHAR_IS_UTF8
        static_jassert (sizeof (wchar_t) == 1);
       #elif JUCE_NATIVE_WCHAR_IS_UTF16
        static_jassert (sizeof (wchar_t) == 2);
       #elif JUCE_NATIVE_WCHAR_IS_UTF32
        static_jassert (sizeof (wchar_t) == 4);
       #else
        #error "native wchar_t size is unknown"
       #endif

        static_jassert (sizeof (EmptyString) == sizeof (StringHolder));
    }
};

namespace NumberToStringConverters
{
    enum
    {
        charsNeededForInt = 32,
        charsNeededForDouble = 48
    };

    template <typename Type>
    static char* printDigits (char* t, Type v) NOEXCEPT
    {
        *--t = 0;

        do
        {
            *--t = '0' + (char) (v % 10);
            v /= 10;

        } while (v > 0);

        return t;
    }

    // pass in a pointer to the END of a buffer..
    char* numberToString (char* t, const int64 n) NOEXCEPT;

    char* numberToString (char* t, uint64 v) NOEXCEPT;

    char* numberToString (char* t, const int n) NOEXCEPT;

    char* numberToString (char* t, unsigned int v) NOEXCEPT;

    struct StackArrayStream  : public std::basic_streambuf<char, std::char_traits<char> >
    {
        explicit StackArrayStream (char* d);
        size_t writeDouble (double n, int numDecPlaces);
    };

    char* doubleToString (char* buffer, const int numChars, double n, int numDecPlaces, size_t& len) NOEXCEPT;

    template <typename IntegerType>
    String::CharPointerType createFromInteger (const IntegerType number)
    {
        char buffer [charsNeededForInt];
        char* const end = buffer + numElementsInArray (buffer);
        char* const start = numberToString (end, number);
        return StringHolder::createFromFixedLength (start, (size_t) (end - start - 1));
    }

    String::CharPointerType createFromDouble (const double number, const int numberOfDecimalPlaces);
}

} // namespace treecore

#endif // TREECORE_STRING_PRIVATE_H