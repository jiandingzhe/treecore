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

#include "treecore/File.h"
#include "treecore/LocalisedStrings.h"
#include "treecore/Memory.h"
#include "treecore/StringRef.h"
#include "treecore/SpinLock.h"
#include "treecore/ScopedPointer.h"

namespace treecore {

LocalisedStrings::LocalisedStrings (const String& fileContents, bool ignoreCase)
{
    loadFromText (fileContents, ignoreCase);
}

LocalisedStrings::LocalisedStrings (const File& fileToLoad, bool ignoreCase)
{
    loadFromText (fileToLoad.loadFileAsString(), ignoreCase);
}

LocalisedStrings::LocalisedStrings (const LocalisedStrings& other)
    : languageName(other.languageName)
    , countryCodes(other.countryCodes)
    , translations(other.translations)
    , fallback(createCopyIfNotNull(other.fallback))
{
}

LocalisedStrings& LocalisedStrings::operator= (const LocalisedStrings& other)
{
    languageName = other.languageName;
    countryCodes = other.countryCodes;
    translations = other.translations;
    fallback = createCopyIfNotNull (other.fallback);
    return *this;
}

LocalisedStrings::~LocalisedStrings()
{
    delete fallback;
}

String LocalisedStrings::translate (const String& text, const String& resultIfNotFound) const
{
    MapType::ConstIterator it(translations);
    if (translations.select(text, it))
    {
        return it.value();
    }
    else if (fallback != nullptr)
    {
        return fallback->translate (text);
    }
    else
    {
        return resultIfNotFound;
    }
}

namespace
{
   #if JUCE_CHECK_MEMORY_LEAKS
    // By using this object to force a LocalisedStrings object to be created
    // before the currentMappings object, we can force the static order-of-destruction to
    // delete the currentMappings object first, which avoids a bogus leak warning.
    // (Oddly, just creating a LocalisedStrings on the stack doesn't work in gcc, it
    // has to be created with 'new' for this to work..)
    struct LeakAvoidanceTrick
    {
        LeakAvoidanceTrick()
        {
            const ScopedPointer<LocalisedStrings> dummy (new LocalisedStrings (String(), false));
        }
    };

    LeakAvoidanceTrick leakAvoidanceTrick;
   #endif

    SpinLock currentMappingsLock;
    ScopedPointer<LocalisedStrings> currentMappings;

    static int findCloseQuote (const String& text, int startPos)
    {
        juce_wchar lastChar = 0;
        String::CharPointerType t (text.getCharPointer() + startPos);

        for (;;)
        {
            const juce_wchar c = t.getAndAdvance();

            if (c == 0 || (c == '"' && lastChar != '\\'))
                break;

            lastChar = c;
            ++startPos;
        }

        return startPos;
    }

    static String unescapeString (const String& s)
    {
        return s.replace ("\\\"", "\"")
                .replace ("\\\'", "\'")
                .replace ("\\t", "\t")
                .replace ("\\r", "\r")
                .replace ("\\n", "\n");
    }
}

void LocalisedStrings::loadFromText (const String& fileContents, bool ignoreCase)
{
    StringArray lines;
    lines.addLines (fileContents);

    for (int i = 0; i < lines.size(); ++i)
    {
        String line (lines[i].trim());

        if (line.startsWithChar ('"'))
        {
            int closeQuote = findCloseQuote (line, 1);

            const String originalText (unescapeString (line.substring (1, closeQuote)));

            if (originalText.isNotEmpty())
            {
                const int openingQuote = findCloseQuote (line, closeQuote + 1);
                closeQuote = findCloseQuote (line, openingQuote + 1);

                const String newText (unescapeString (line.substring (openingQuote + 1, closeQuote)));

                if (newText.isNotEmpty())
                    translations.set(originalText, newText);
            }
        }
        else if (line.startsWithIgnoreCase ("language:"))
        {
            languageName = line.substring (9).trim();
        }
        else if (line.startsWithIgnoreCase ("countries:"))
        {
            countryCodes.addTokens (line.substring (10).trim(), true);
            countryCodes.trim();
            countryCodes.removeEmptyStrings();
        }
    }
}

void LocalisedStrings::addStrings (const LocalisedStrings& other)
{
    jassert (languageName == other.languageName);
    jassert (countryCodes == other.countryCodes);

    MapType::ConstIterator i_other(other.translations);
    while (i_other.next())
    {
        translations[i_other.key()] = i_other.value();
    }
}

void LocalisedStrings::setFallback (LocalisedStrings* f)
{
    fallback = f;
}

//==============================================================================
void LocalisedStrings::setCurrentMappings (LocalisedStrings* newTranslations)
{
    const SpinLock::ScopedLockType sl (currentMappingsLock);
    currentMappings = newTranslations;
}

LocalisedStrings* LocalisedStrings::getCurrentMappings()
{
    return currentMappings;
}

String LocalisedStrings::translateWithCurrentMappings (const String& text)  { return treecore::translate (text); }
String LocalisedStrings::translateWithCurrentMappings (const char* text)    { return treecore::translate (text); }

JUCE_API String translate (const String& text)       { return treecore::translate (text, text); }
JUCE_API String translate (const char* text)         { return treecore::translate (String (text)); }
JUCE_API String translate (CharPointer_UTF8 text)    { return treecore::translate (String (text)); }

JUCE_API String translate (const String& text, const String& resultIfNotFound)
{
    const SpinLock::ScopedLockType sl (currentMappingsLock);

    if (const LocalisedStrings* const mappings = LocalisedStrings::getCurrentMappings())
        return mappings->translate (text, resultIfNotFound);

    return resultIfNotFound;
}

}
