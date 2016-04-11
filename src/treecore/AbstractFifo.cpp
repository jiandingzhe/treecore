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

#include "treecore/AbstractFifo.h"

namespace treecore
{

AbstractFifo::AbstractFifo ( const int capacity ) noexcept
    : bufferSize( capacity )
{
    treecore_assert( bufferSize > 0 );
}

AbstractFifo::~AbstractFifo() {}

int AbstractFifo::getTotalSize() const noexcept           { return bufferSize; }
int AbstractFifo::getFreeSpace() const noexcept           { return bufferSize - getNumReady() - 1; }

int AbstractFifo::getNumReady() const noexcept
{
    const int vs = validStart.load();
    const int ve = validEnd.load();
    return ve >= vs ? (ve - vs) : ( bufferSize - (vs - ve) );
}

void AbstractFifo::reset() noexcept
{
    validEnd   = 0;
    validStart = 0;
}

void AbstractFifo::setTotalSize( int newSize ) noexcept
{
    treecore_assert( newSize > 0 );
    reset();
    bufferSize = newSize;
}

//==============================================================================
void AbstractFifo::prepareToWrite( int numToWrite, int& startIndex1, int& blockSize1, int& startIndex2, int& blockSize2 ) const noexcept
{
    const int vs = validStart.load();
    const int ve = validEnd.load();

    const int freeSpace = ve >= vs ? ( bufferSize - (ve - vs) ) : (vs - ve);
    numToWrite = jmin( numToWrite, freeSpace - 1 );

    if (numToWrite <= 0)
    {
        startIndex1 = 0;
        startIndex2 = 0;
        blockSize1  = 0;
        blockSize2  = 0;
    }
    else
    {
        startIndex1 = ve;
        startIndex2 = 0;
        blockSize1  = jmin( bufferSize - ve, numToWrite );
        numToWrite -= blockSize1;
        blockSize2  = numToWrite <= 0 ? 0 : jmin( numToWrite, vs );
    }
}

void AbstractFifo::finishedWrite( int numWritten ) noexcept
{
    treecore_assert( numWritten >= 0 && numWritten < bufferSize );
    int newEnd = validEnd.load() + numWritten;
    if (newEnd >= bufferSize)
        newEnd -= bufferSize;

    validEnd = newEnd;
}

void AbstractFifo::prepareToRead( int numWanted, int& startIndex1, int& blockSize1, int& startIndex2, int& blockSize2 ) const noexcept
{
    const int vs = validStart.load();
    const int ve = validEnd.load();

    const int numReady = ve >= vs ? (ve - vs) : ( bufferSize - (vs - ve) );
    numWanted = jmin( numWanted, numReady );

    if (numWanted <= 0)
    {
        startIndex1 = 0;
        startIndex2 = 0;
        blockSize1  = 0;
        blockSize2  = 0;
    }
    else
    {
        startIndex1 = vs;
        startIndex2 = 0;
        blockSize1  = jmin( bufferSize - vs, numWanted );
        numWanted  -= blockSize1;
        blockSize2  = numWanted <= 0 ? 0 : jmin( numWanted, ve );
    }
}

void AbstractFifo::finishedRead( int numRead ) noexcept
{
    treecore_assert( numRead >= 0 && numRead <= bufferSize );

    int newStart = validStart.load() + numRead;
    if (newStart >= bufferSize)
        newStart -= bufferSize;

    validStart = newStart;
}

} // namespace treecore
