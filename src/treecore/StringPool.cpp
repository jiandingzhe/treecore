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

#include "treecore/StringPool.h"
#include "treecore/Time.h"

namespace treecore {

static const int minNumberOfStringsForGarbageCollection = 300;
static const uint32 garbageCollectionInterval = 30000;

static inline const char* _add_or_get_it_(HashSet<String>& pool, const String& input)
{
    HashSet<String>::Iterator it(pool);
    pool.insertOrSelect(input, it);
    return it.content().getCharPointer().getAddress();
}

StringPool::StringPool() noexcept
{
}

StringPool::~StringPool() {}

const char* StringPool::getPooledString (const char* const newString)
{
    if (newString == nullptr || *newString == 0)
        return CharPointer_UTF8(nullptr);

    const ScopedLock sl(m_lock);
    return _add_or_get_it_(m_strings, CharPointer_UTF8(newString));
}

const char* StringPool::getPooledString (String::CharPointerType start, String::CharPointerType end)
{
    if (start.isEmpty() || start == end)
        return CharPointer_UTF8(nullptr);

    const ScopedLock sl(m_lock);
    return _add_or_get_it_(m_strings, String(start, end));
}

const char* StringPool::getPooledString (StringRef newString)
{
    if (newString.isEmpty())
        return CharPointer_UTF8(nullptr);

    const ScopedLock sl(m_lock);
    return _add_or_get_it_(m_strings, newString.text);
}

const char* StringPool::getPooledString (const String& newString)
{
    if (newString.isEmpty())
        return CharPointer_UTF8(nullptr);

    const ScopedLock sl(m_lock);
    return _add_or_get_it_(m_strings, newString);
}


} // namespace treecore
