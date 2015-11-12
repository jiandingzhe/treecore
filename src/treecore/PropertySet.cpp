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

#include "treecore/PropertySet.h"
#include "treecore/Variant.h"
#include "treecore/XmlElement.h"
#include "treecore/XmlDocument.h"

namespace treecore {

PropertySet::PropertySet (const bool ignoreCaseOfKeyNames)
    : properties (ignoreCaseOfKeyNames),
      fallbackProperties (nullptr),
      ignoreCaseOfKeys (ignoreCaseOfKeyNames)
{
}

PropertySet::PropertySet (const PropertySet& other)
    : properties (other.properties),
      fallbackProperties (other.fallbackProperties),
      ignoreCaseOfKeys (other.ignoreCaseOfKeys)
{
}

PropertySet& PropertySet::operator= (const PropertySet& other)
{
    properties = other.properties;
    fallbackProperties = other.fallbackProperties;
    ignoreCaseOfKeys = other.ignoreCaseOfKeys;

    propertyChanged();
    return *this;
}

PropertySet::~PropertySet()
{
}

void PropertySet::clear()
{
    const ScopedLock sl (lock);

    if (properties.size() > 0)
    {
        properties.clear();
        propertyChanged();
    }
}

const String& PropertySet::getValue (const String& keyName, const String& defaultValue) const noexcept
{
    const ScopedLock sl(lock);

    MapType::ConstIterator i_key(properties);
    if (properties.select(keyName, i_key))
    {
        return i_key.value();
    }
    else
    {
        return fallbackProperties
                ? fallbackProperties->getValue(keyName, defaultValue)
                : defaultValue;
    }
}

int PropertySet::getIntValue (const String& keyName, const int defaultValue) const noexcept
{
    const ScopedLock sl(lock);
    MapType::ConstIterator i_key(properties);
    if (properties.select(keyName, i_key))
    {
        return i_key.value().getIntValue();
    }
    else
    {
        return fallbackProperties
                ? fallbackProperties->getIntValue(keyName, defaultValue)
                : defaultValue;
    }
}

double PropertySet::getDoubleValue (const String& keyName, const double defaultValue) const noexcept
{
    const ScopedLock sl(lock);
    MapType::ConstIterator i_key(properties);
    if (properties.select(keyName, i_key))
    {
        return i_key.value().getDoubleValue();
    }
    else
    {
        return fallbackProperties
                ? fallbackProperties->getDoubleValue(keyName, defaultValue)
                : defaultValue;
    }
}

bool PropertySet::getBoolValue (const String& keyName, const bool defaultValue) const noexcept
{
    const ScopedLock sl(lock);
    MapType::ConstIterator i_key(properties);
    if (properties.select(keyName, i_key))
    {
        return i_key.value().getIntValue() != 0;
    }
    else
    {
        return fallbackProperties
                ? fallbackProperties->getIntValue(keyName, defaultValue) != 0
                : defaultValue;
    }
}

XmlElement* PropertySet::getXmlValue (const String& keyName) const
{
    return XmlDocument::parse (getValue (keyName));
}

void PropertySet::setValue (const String& keyName, const var& v)
{
    if (keyName.isNotEmpty())
    {
        const ScopedLock sl (lock);
        String v_str(v.toString());

        MapType::Iterator i(properties);
        bool inserted = properties.insertOrSelect(keyName, v_str, i);

        bool modified = false;
        if (!inserted && i.value() != v_str)
        {
            modified = true;
            i.value() = v_str;
        }

        if (inserted || modified)
            propertyChanged();
    }
}

void PropertySet::removeValue (const String& keyName)
{
    if (keyName.isNotEmpty())
    {
        const ScopedLock sl (lock);

        if (properties.remove(keyName))
            propertyChanged();
    }
}

void PropertySet::setValue (const String& keyName, const XmlElement* const xml)
{
    setValue (keyName, xml == nullptr ? var()
                                      : var (xml->createDocument ("", true)));
}

bool PropertySet::containsKey (const String& keyName) const noexcept
{
    const ScopedLock sl (lock);
    return properties.contains(keyName);
}

void PropertySet::addAllPropertiesFrom (const PropertySet& source)
{
    const ScopedLock sl (source.getLock());

    MapType::ConstIterator i_source(source.properties);
    while (i_source.next())
    {
        setValue(i_source.key(), i_source.value());
    }
}

void PropertySet::setFallbackPropertySet (PropertySet* fallbackProperties_) noexcept
{
    const ScopedLock sl (lock);
    fallbackProperties = fallbackProperties_;
}

XmlElement* PropertySet::createXml (const String& nodeName) const
{
    const ScopedLock sl (lock);
    XmlElement* const xml = new XmlElement (nodeName);

    MapType::ConstIterator i(properties);
    while (i.next())
    {
        XmlElement* const e = xml->createNewChildElement ("VALUE");
        e->setAttribute ("name", i.key());
        e->setAttribute ("val", i.value());
    }

    return xml;
}

void PropertySet::restoreFromXml (const XmlElement& xml)
{
    const ScopedLock sl (lock);
    clear();

    forEachXmlChildElementWithTagName (xml, e, "VALUE")
    {
        if (e->hasAttribute ("name")
                && e->hasAttribute ("val"))
        {
            properties.set (e->getStringAttribute ("name"),
                            e->getStringAttribute ("val"));
        }
    }

    if (properties.size() > 0)
        propertyChanged();
}

void PropertySet::propertyChanged()
{
}

}
