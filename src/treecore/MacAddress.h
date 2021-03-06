﻿/*
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

#ifndef TREECORE_MACADDRESS_H
#define TREECORE_MACADDRESS_H

#include "treecore/Array.h"

namespace treecore
{

class String;
/**
    Represents a MAC network card adapter address ID.
*/
class TREECORE_SHARED_API  MacAddress
{
public:
    //==============================================================================
    /** Populates a list of the MAC addresses of all the available network cards. */
    static void findAllAddresses (Array<MacAddress>& results);

    //==============================================================================
    /** Creates a null address (00-00-00-00-00-00). */
    MacAddress();

    /** Creates a copy of another address. */
    MacAddress (const MacAddress&);

    /** Creates a copy of another address. */
    MacAddress& operator= (const MacAddress&);

    /** Creates an address from 6 bytes. */
    explicit MacAddress (const uint8 bytes[6]);

    /** Returns a pointer to the 6 bytes that make up this address. */
    const uint8* getBytes() const noexcept        { return address; }

    /** Returns a dash-separated string in the form "11-22-33-44-55-66" */
    String toString() const;

    /** Returns the address in the lower 6 bytes of an int64.

        This uses a little-endian arrangement, with the first byte of the address being
        stored in the least-significant byte of the result value.
    */
    int64 toInt64() const noexcept;

    /** Returns true if this address is null (00-00-00-00-00-00). */
    bool isNull() const noexcept;

    bool operator== (const MacAddress&) const noexcept;
    bool operator!= (const MacAddress&) const noexcept;

    //==============================================================================
private:
    uint8 address[6];
};

} // namespace treecore

#endif   // TREECORE_MACADDRESS_H
