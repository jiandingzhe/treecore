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

#include "treecore/MemoryInputStream.h"
#include "treecore/MemoryBlock.h"

namespace treecore {

MemoryInputStream::MemoryInputStream (const void* const sourceData,
                                      const size_t sourceDataSize,
                                      const bool keepInternalCopy)
    : data (sourceData),
      dataSize (sourceDataSize),
      position (0)
{
    if (keepInternalCopy)
        createInternalCopy();
}

MemoryInputStream::MemoryInputStream (const MemoryBlock& sourceData,
                                      const bool keepInternalCopy)
    : data (sourceData.getData()),
      dataSize (sourceData.getSize()),
      position (0)
{
    if (keepInternalCopy)
        createInternalCopy();
}

void MemoryInputStream::createInternalCopy()
{
    internalCopy.malloc (dataSize);
    memcpy (internalCopy, data, dataSize);
    data = internalCopy;
}

MemoryInputStream::~MemoryInputStream()
{
}

int64 MemoryInputStream::getTotalLength()
{
    return (int64) dataSize;
}

int MemoryInputStream::read (void* const buffer, const int howMany)
{
    treecore_assert (buffer != nullptr && howMany >= 0);

    const int num = jmin (howMany, (int) (dataSize - position));
    if (num <= 0)
        return 0;

    memcpy (buffer, addBytesToPointer (data, position), (size_t) num);
    position += (unsigned int) num;
    return num;
}

bool MemoryInputStream::isExhausted()
{
    return position >= dataSize;
}

bool MemoryInputStream::setPosition (const int64 pos)
{
    position = (size_t) jlimit ((int64) 0, (int64) dataSize, pos);
    return true;
}

int64 MemoryInputStream::getPosition()
{
    return (int64) position;
}

} // namespace treecore
