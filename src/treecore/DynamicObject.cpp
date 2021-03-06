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

#include "treecore/DynamicObject.h"
#include "treecore/JSON.h"
#include "treecore/NewLine.h"
#include "treecore/OutputStream.h"

namespace treecore {

DynamicObject::DynamicObject()
{}

DynamicObject::DynamicObject ( const DynamicObject& other )
    : RefCountObject(), properties( other.properties )
{}

DynamicObject::~DynamicObject()
{}

bool DynamicObject::hasProperty( const Identifier& propertyName ) const
{
    const var* const v = properties.getVarPointer( propertyName );
    return v != nullptr && !v->isMethod();
}

var DynamicObject::getProperty( const Identifier& propertyName ) const
{
    return properties [propertyName];
}

void DynamicObject::setProperty( const Identifier& propertyName, const var& newValue )
{
    properties.set( propertyName, newValue );
}

void DynamicObject::removeProperty( const Identifier& propertyName )
{
    properties.remove( propertyName );
}

bool DynamicObject::hasMethod( const Identifier& methodName ) const
{
    return getProperty( methodName ).isMethod();
}

var DynamicObject::invokeMethod( Identifier method, const var::NativeFunctionArgs& args )
{
    if ( var::NativeFunction function = properties [method].getNativeFunction() )
        return function( args );

    return var();
}

void DynamicObject::setDynamicMethod( Identifier name, var::NativeFunction function )
{
    properties.set( name, var( function ) );
}

void DynamicObject::clear()
{
    properties.clear();
}

void DynamicObject::cloneAllProperties()
{
    NamedValueSet::MapType& data = properties.getValues();
    NamedValueSet::MapType::Iterator i( data );

    while ( i.next() )
        i.value() = i.value().clone();
}

DynamicObject::Ptr DynamicObject::clone()
{
    Ptr d( new DynamicObject( *this ) );
    d->cloneAllProperties();
    return d;
}

struct IdentifierSorter
{
    int compareElements( Identifier a, Identifier b )
    {
        return a.toString().compare( b.toString() );
    }
};

void DynamicObject::writeAsJSON( OutputStream& out, const int indentLevel, const bool allOnOneLine )
{
    out << '{';
    if (!allOnOneLine)
        out << newLine;

    const int numValues = properties.size();
    int count = 0;
    NamedValueSet::MapType& data = properties.getValues();

    Array<Identifier> keys = data.getAllKeys();
    IdentifierSorter sorter;
    keys.sort( sorter );

    for (const Identifier& key : keys)
    {
        count++;
        if (!allOnOneLine)
            JSONFormatter::writeSpaces( out, indentLevel + JSONFormatter::indentSize );

        out << '"';
        JSONFormatter::writeString( out, key.toString().getCharPointer() );
        out << "\": ";
        JSONFormatter::write( out, data[key], indentLevel + JSONFormatter::indentSize, allOnOneLine );

        if (count < numValues)
        {
            if (allOnOneLine)
                out << ", ";
            else
                out << ',' << newLine;
        }
        else if (!allOnOneLine)
            out << newLine;
    }

    if (!allOnOneLine)
        JSONFormatter::writeSpaces( out, indentLevel );

    out << '}';
}

}
