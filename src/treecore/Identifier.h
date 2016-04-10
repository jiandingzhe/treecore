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

#ifndef JUCE_IDENTIFIER_H_INCLUDED
#define JUCE_IDENTIFIER_H_INCLUDED

#include "treecore/String.h"
#include "treecore/StringRef.h"

class TestFramework;

namespace treecore {

//==============================================================================
/**
    Represents a string identifier, designed for accessing properties by name.

    Comparing two Identifier objects is very fast (an O(1) operation), but creating
    them can be slower than just using a String directly, so the optimal way to use them
    is to keep some static Identifier objects for the things you use often.

    @see NamedValueSet, ValueTree
*/
class TREECORE_SHARED_API  Identifier
{
    friend class ::TestFramework;

public:
    /** Creates a null identifier. */
    Identifier() noexcept;

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (const char* name);

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (const String& name);

    /** Creates an identifier with a specified name.
        Because this name may need to be used in contexts such as script variables or XML
        tags, it must only contain ascii letters and digits, or the underscore character.
    */
    Identifier (String::CharPointerType nameStart, String::CharPointerType nameEnd);

    /** Creates a copy of another identifier. */
    Identifier (const Identifier& other) noexcept;

    /** Creates a copy of another identifier. */
    Identifier& operator= (const Identifier other) noexcept;

    /** Destructor */
    ~Identifier() noexcept;

    /** Compares two identifiers. This is a very fast operation. */
    inline bool operator== (Identifier other) const noexcept
    {
        return m_name == other.m_name;
    }

    /** Compares two identifiers. This is a very fast operation. */
    inline bool operator!= (Identifier other) const noexcept
    {
        return m_name != other.m_name;
    }

    /** Compares the identifier with a string. */
    inline bool operator== (StringRef other) const noexcept
    {
        return String(m_name) == other;
    }

    /** Compares the identifier with a string. */
    inline bool operator!= (StringRef other) const noexcept
    {
        return String(m_name) != other;
    }

    /** Returns this identifier as a string. */
    String toString() const noexcept
    {
        return m_name;
    }

    /**
     * @brief return raw string pointer of this Identifier
     * @return char pointer
     */
    const char* getPtr() const noexcept
    {
        return m_name;
    }

    /** Returns this identifier's raw string pointer. */
    operator String::CharPointerType() const noexcept
    {
        return String(m_name).getCharPointer();
    }

    /** Returns this identifier's raw string pointer. */
    String::CharPointerType getCharPointer() const noexcept
    {
        return String(m_name).getCharPointer();
    }

    /** Returns this identifier as a StringRef. */
    operator StringRef() const noexcept
    {
        return m_name;
    }

    /** Returns true if this Identifier is not null */
    bool isValid() const noexcept
    {
        return m_name != nullptr;
    }

    /** Returns true if this Identifier is null */
    bool isNull() const noexcept
    {
        return m_name == nullptr;
    }

    operator bool () const noexcept
    {
        return m_name != nullptr;
    }

    /** A null identifier. */
    static Identifier null;

private:
    const char* m_name = nullptr;
};

}

#endif   // JUCE_IDENTIFIER_H_INCLUDED
