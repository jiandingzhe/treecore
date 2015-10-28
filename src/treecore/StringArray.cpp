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

#include "treecore/StringArray.h"
#include "treecore/StringRef.h"

namespace treecore {

StringArray::StringArray() noexcept
{
}

StringArray::StringArray (const StringArray& other)
    : strings (other.strings)
{
}

StringArray::StringArray (StringArray&& other) noexcept
    : strings (static_cast <Array <String>&&> (other.strings))
{
}

StringArray::StringArray (const String& firstValue)
{
    strings.add (firstValue);
}

StringArray::StringArray (const String* initialStrings, int numberOfStrings)
{
    strings.addArray (initialStrings, numberOfStrings);
}

StringArray::StringArray (const char* const* initialStrings)
{
    strings.addNullTerminatedArray (initialStrings);
}

StringArray::StringArray (const char* const* initialStrings, int numberOfStrings)
{
    strings.addArray (initialStrings, numberOfStrings);
}

StringArray::StringArray (const wchar_t* const* initialStrings)
{
    strings.addNullTerminatedArray (initialStrings);
}

StringArray::StringArray (const wchar_t* const* initialStrings, int numberOfStrings)
{
    strings.addArray (initialStrings, numberOfStrings);
}

StringArray& StringArray::operator= (const StringArray& other)
{
    strings = other.strings;
    return *this;
}

StringArray& StringArray::operator= (StringArray&& other) noexcept
{
    strings = static_cast <Array<String>&&> (other.strings);
    return *this;
}

StringArray::StringArray (const std::initializer_list<const char*>& stringList)
{
    strings.addArray (stringList);
}


StringArray::~StringArray()
{
}

bool StringArray::operator== (const StringArray& other) const noexcept
{
    return strings == other.strings;
}

bool StringArray::operator!= (const StringArray& other) const noexcept
{
    return ! operator== (other);
}

void StringArray::addIfNotAlreadyThere (const String& newString, const bool ignoreCase)
{
    if (! contains (newString, ignoreCase))
        add (newString);
}

void StringArray::addArray (const StringArray& otherArray, int startIndex, int numElementsToAdd)
{
    if (startIndex < 0)
    {
        jassertfalse;
        startIndex = 0;
    }

    if (numElementsToAdd < 0 || startIndex + numElementsToAdd > otherArray.size())
        numElementsToAdd = otherArray.size() - startIndex;

    while (--numElementsToAdd >= 0)
        strings.add (otherArray.strings[startIndex++]);
}


int StringArray::indexOf (StringRef stringToLookFor, const bool ignoreCase, int i) const noexcept
{
    if (i < 0)
        i = 0;

    const int numElements = size();

    if (ignoreCase)
    {
        for (; i < numElements; ++i)
            if (strings[i].equalsIgnoreCase (stringToLookFor))
                return i;
    }
    else
    {
        for (; i < numElements; ++i)
            if (stringToLookFor == strings[i])
                return i;
    }

    return -1;
}


void StringArray::removeString (StringRef stringToRemove, const bool ignoreCase)
{
    if (ignoreCase)
    {
        for (int i = size(); --i >= 0;)
            if (strings[i].equalsIgnoreCase (stringToRemove))
                strings.remove (i);
    }
    else
    {
        for (int i = size(); --i >= 0;)
            if (stringToRemove == strings[i])
                strings.remove (i);
    }
}

//==============================================================================
void StringArray::removeEmptyStrings (const bool removeWhitespaceStrings)
{
    if (removeWhitespaceStrings)
    {
        for (int i = size(); --i >= 0;)
            if (! strings[i].containsNonWhitespaceChars())
                strings.remove (i);
    }
    else
    {
        for (int i = size(); --i >= 0;)
            if (strings[i].isEmpty())
                strings.remove (i);
    }
}

void StringArray::trim()
{
    for (int i = size(); --i >= 0;)
    {
        String& s = strings[i];
        s = s.trim();
    }
}

//==============================================================================
struct InternalStringArrayComparator_CaseSensitive
{
    static int compareElements (String& s1, String& s2) noexcept    { return s1.compare (s2); }
};

struct InternalStringArrayComparator_CaseInsensitive
{
    static int compareElements (String& s1, String& s2) noexcept    { return s1.compareIgnoreCase (s2); }
};

struct InternalStringArrayComparator_Natural
{
    static int compareElements (String& s1, String& s2) noexcept    { return s1.compareNatural (s2); }
};

void StringArray::sort (const bool ignoreCase)
{
    if (ignoreCase)
    {
        InternalStringArrayComparator_CaseInsensitive comp;
        strings.sort (comp);
    }
    else
    {
        InternalStringArrayComparator_CaseSensitive comp;
        strings.sort (comp);
    }
}

void StringArray::sortNatural()
{
    InternalStringArrayComparator_Natural comp;
    strings.sort (comp);
}

//==============================================================================
String StringArray::joinIntoString (StringRef separator, int start, int numberToJoin) const
{
    const int last = (numberToJoin < 0) ? size()
                                        : jmin (size(), start + numberToJoin);

    if (start < 0)
        start = 0;

    if (start >= last)
        return String();

    if (start == last - 1)
        return strings[start];

    const size_t separatorBytes = separator.text.sizeInBytes() - sizeof (String::CharPointerType::CharType);
    size_t bytesNeeded = separatorBytes * (size_t) (last - start - 1);

    for (int i = start; i < last; ++i)
        bytesNeeded += strings[i].getCharPointer().sizeInBytes() - sizeof (String::CharPointerType::CharType);

    String result;
    result.preallocateBytes (bytesNeeded);

    String::CharPointerType dest (result.getCharPointer());

    while (start < last)
    {
        const String& s = strings[start];

        if (! s.isEmpty())
            dest.writeAll (s.getCharPointer());

        if (++start < last && separatorBytes > 0)
            dest.writeAll (separator.text);
    }

    dest.writeNull();

    return result;
}

int StringArray::addTokens (StringRef text, StringRef breakCharacters, StringRef quoteCharacters)
{
    int num = 0;

    if (text.isNotEmpty())
    {
        for (String::CharPointerType t (text.text);;)
        {
            String::CharPointerType tokenEnd (CharacterFunctions::findEndOfToken (t,
                                                                                  breakCharacters.text,
                                                                                  quoteCharacters.text));
            strings.add (String (t, tokenEnd));
            ++num;

            if (tokenEnd.isEmpty())
                break;

            t = ++tokenEnd;
        }
    }

    return num;
}

int StringArray::addLines (StringRef sourceText)
{
    int numLines = 0;
    String::CharPointerType text (sourceText.text);
    bool finished = text.isEmpty();

    while (! finished)
    {
        for (String::CharPointerType startOfLine (text);;)
        {
            const String::CharPointerType endOfLine (text);

            switch (text.getAndAdvance())
            {
                case 0:     finished = true; break;
                case '\n':  break;
                case '\r':  if (*text == '\n') ++text; break;
                default:    continue;
            }

            strings.add (String (startOfLine, endOfLine));
            ++numLines;
            break;
        }
    }

    return numLines;
}

StringArray StringArray::fromTokens (StringRef stringToTokenise, bool preserveQuotedStrings)
{
    StringArray s;
    s.addTokens (stringToTokenise, preserveQuotedStrings);
    return s;
}

StringArray StringArray::fromTokens (StringRef stringToTokenise,
                                     StringRef breakCharacters,
                                     StringRef quoteCharacters)
{
    StringArray s;
    s.addTokens (stringToTokenise, breakCharacters, quoteCharacters);
    return s;
}

StringArray StringArray::fromLines (StringRef stringToBreakUp)
{
    StringArray s;
    s.addLines (stringToBreakUp);
    return s;
}

//==============================================================================
void StringArray::removeDuplicates (const bool ignoreCase)
{
    for (int i = 0; i < size() - 1; ++i)
    {
        const String s (strings[i]);

        for (int nextIndex = i + 1;;)
        {
            nextIndex = indexOf (s, ignoreCase, nextIndex);

            if (nextIndex < 0)
                break;

            strings.remove (nextIndex);
        }
    }
}

void StringArray::appendNumbersToDuplicates (const bool ignoreCase,
                                             const bool appendNumberToFirstInstance,
                                             CharPointer_UTF8 preNumberString,
                                             CharPointer_UTF8 postNumberString)
{
    CharPointer_UTF8 defaultPre (" ("), defaultPost (")");

    if (preNumberString.getAddress() == nullptr)
        preNumberString = defaultPre;

    if (postNumberString.getAddress() == nullptr)
        postNumberString = defaultPost;

    for (int i = 0; i < size() - 1; ++i)
    {
        String& s = strings[i];

        int nextIndex = indexOf (s, ignoreCase, i + 1);

        if (nextIndex >= 0)
        {
            const String original (s);

            int number = 0;

            if (appendNumberToFirstInstance)
                s = original + String (preNumberString) + String (++number) + String (postNumberString);
            else
                ++number;

            while (nextIndex >= 0)
            {
                strings[nextIndex] = (*this)[nextIndex] + String (preNumberString) + String (++number) + String (postNumberString);
                nextIndex = indexOf (original, ignoreCase, nextIndex + 1);
            }
        }
    }
}

}
