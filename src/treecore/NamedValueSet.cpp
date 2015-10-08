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

#include "treecore/Identifier.h"
#include "treecore/MemoryBlock.h"
#include "treecore/NamedValueSet.h"
#include "treecore/Variant.h"
#include "treecore/XmlElement.h"

namespace treecore {


//==============================================================================
NamedValueSet::NamedValueSet() noexcept
{
}

NamedValueSet::NamedValueSet (const NamedValueSet& other)
   : values (other.values)
{
}

NamedValueSet& NamedValueSet::operator= (const NamedValueSet& other)
{
    clear();
    values = other.values;
    return *this;
}

NamedValueSet::NamedValueSet (NamedValueSet&& other) noexcept
    : values(static_cast<MapType&&>(other.values))
{
}

NamedValueSet& NamedValueSet::operator= (NamedValueSet&& other) noexcept
{
    other.values.swapWith (values);
    return *this;
}

NamedValueSet::~NamedValueSet()
{
    clear();
}

void NamedValueSet::clear()
{
    values.clear();
}

bool NamedValueSet::operator== (const NamedValueSet& other) const
{
    return values == other.values;
}

bool NamedValueSet::operator!= (const NamedValueSet& other) const
{
    return values != other.values;
}

int NamedValueSet::size() const noexcept
{
    return values.size();
}

const var& NamedValueSet::operator[] (const Identifier& name) const
{
    if (const var* v = getVarPointer (name))
        return *v;

    return var::null;
}

var NamedValueSet::getWithDefault (const Identifier& name, const var& defaultReturnValue) const
{
    if (const var* const v = getVarPointer (name))
        return *v;

    return defaultReturnValue;
}

var* NamedValueSet::getVarPointer (const Identifier& name) const noexcept
{
    MapType::ConstIterator i(values);
    if (values.select(name, i))
    {
        return const_cast<var*>(&i.value());
    }
    return nullptr;
}

bool NamedValueSet::set (Identifier name, var&& newValue)
{
    MapType::Iterator i(values);
    if (!values.insertOrSelect(name, newValue, i))
    {
        if (i.value().equalsWithSameType(newValue))
            return false;

        i.value() = newValue;
    }

    return true;
}

bool NamedValueSet::set (Identifier name, const var& newValue)
{
    MapType::Iterator i(values);
    if (!values.insertOrSelect(name, newValue, i))
    {
        if (i.value().equalsWithSameType(newValue))
            return false;

        i.value() = newValue;
    }

    return true;
}

bool NamedValueSet::contains (const Identifier& name) const
{
    return values.contains(name);
}

bool NamedValueSet::remove (const Identifier& name)
{
    return values.remove(name);
}

void NamedValueSet::setFromXmlAttributes (const XmlElement& xml)
{
    values.clear();

    for (const XmlElement::XmlAttributeNode* att = xml.attributes; att != nullptr; att = att->nextListItem)
    {
        if (att->name.toString().startsWith ("base64:"))
        {
            MemoryBlock mb;

            if (mb.fromBase64Encoding(att->value))
                values[att->name.toString().substring (7)] = var(mb);
        }
        else
        {
            values[att->name] = var(att->value);
        }
    }
}

void NamedValueSet::copyToXmlAttributes (XmlElement& xml) const
{
    MapType::ConstIterator i(values);

    while (i.next())
    {
        if (const MemoryBlock* mb = i.value().getBinaryData())
        {
            xml.setAttribute ("base64:" + i.key().toString(), mb->toBase64Encoding());
        }
        else
        {
            // These types can't be stored as XML!
            jassert (! i.value().isObject());
            jassert (! i.value().isMethod());
            jassert (! i.value().isArray());

            xml.setAttribute (i.key().toString(),
                              i.value().toString());
        }
    }
}

}
