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

#include "treecore/LocalisedStrings.h"
#include "treecore/RelativeTime.h"
#include "treecore/StringRef.h"

namespace treecore {

RelativeTime::RelativeTime (const double secs) NOEXCEPT           : numSeconds (secs) {}
RelativeTime::RelativeTime (const RelativeTime& other) NOEXCEPT   : numSeconds (other.numSeconds) {}
RelativeTime::~RelativeTime() NOEXCEPT {}

//==============================================================================
RelativeTime RelativeTime::milliseconds (const int milliseconds) NOEXCEPT   { return RelativeTime (milliseconds * 0.001); }
RelativeTime RelativeTime::milliseconds (const int64 milliseconds) NOEXCEPT { return RelativeTime (milliseconds * 0.001); }
RelativeTime RelativeTime::seconds (double s) NOEXCEPT                      { return RelativeTime (s); }
RelativeTime RelativeTime::minutes (const double numberOfMinutes) NOEXCEPT  { return RelativeTime (numberOfMinutes * 60.0); }
RelativeTime RelativeTime::hours (const double numberOfHours) NOEXCEPT      { return RelativeTime (numberOfHours * (60.0 * 60.0)); }
RelativeTime RelativeTime::days (const double numberOfDays) NOEXCEPT        { return RelativeTime (numberOfDays  * (60.0 * 60.0 * 24.0)); }
RelativeTime RelativeTime::weeks (const double numberOfWeeks) NOEXCEPT      { return RelativeTime (numberOfWeeks * (60.0 * 60.0 * 24.0 * 7.0)); }

//==============================================================================
int64 RelativeTime::inMilliseconds() const NOEXCEPT { return (int64) (numSeconds * 1000.0); }
double RelativeTime::inMinutes() const NOEXCEPT     { return numSeconds / 60.0; }
double RelativeTime::inHours() const NOEXCEPT       { return numSeconds / (60.0 * 60.0); }
double RelativeTime::inDays() const NOEXCEPT        { return numSeconds / (60.0 * 60.0 * 24.0); }
double RelativeTime::inWeeks() const NOEXCEPT       { return numSeconds / (60.0 * 60.0 * 24.0 * 7.0); }

//==============================================================================
RelativeTime& RelativeTime::operator= (const RelativeTime& other) NOEXCEPT      { numSeconds = other.numSeconds; return *this; }

RelativeTime RelativeTime::operator+= (RelativeTime t) NOEXCEPT     { numSeconds += t.numSeconds; return *this; }
RelativeTime RelativeTime::operator-= (RelativeTime t) NOEXCEPT     { numSeconds -= t.numSeconds; return *this; }
RelativeTime RelativeTime::operator+= (const double secs) NOEXCEPT  { numSeconds += secs; return *this; }
RelativeTime RelativeTime::operator-= (const double secs) NOEXCEPT  { numSeconds -= secs; return *this; }

RelativeTime operator+ (RelativeTime t1, RelativeTime t2) NOEXCEPT  { return t1 += t2; }
RelativeTime operator- (RelativeTime t1, RelativeTime t2) NOEXCEPT  { return t1 -= t2; }

bool operator== (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() == t2.inSeconds(); }
bool operator!= (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() != t2.inSeconds(); }
bool operator>  (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() >  t2.inSeconds(); }
bool operator<  (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() <  t2.inSeconds(); }
bool operator>= (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() >= t2.inSeconds(); }
bool operator<= (RelativeTime t1, RelativeTime t2) NOEXCEPT       { return t1.inSeconds() <= t2.inSeconds(); }

//==============================================================================
static void translateTimeField (String& result, int n, const char* singular, const char* plural)
{
    result << TRANS (n == 1 ? singular : plural)
                .replace (n == 1 ? "1" : "2", String (n))
           << ' ';
}

String RelativeTime::getDescription (const String& returnValueForZeroTime) const
{
    if (numSeconds < 0.001 && numSeconds > -0.001)
        return returnValueForZeroTime;

    String result;
    result.preallocateBytes (32);

    if (numSeconds < 0)
        result << '-';

    int fieldsShown = 0;
    int n = std::abs ((int) inWeeks());
    if (n > 0)
    {
        translateTimeField (result, n, NEEDS_TRANS("1 week"), NEEDS_TRANS("2 weeks"));
        ++fieldsShown;
    }

    n = std::abs ((int) inDays()) % 7;
    if (n > 0)
    {
        translateTimeField (result, n, NEEDS_TRANS("1 day"), NEEDS_TRANS("2 days"));
        ++fieldsShown;
    }

    if (fieldsShown < 2)
    {
        n = std::abs ((int) inHours()) % 24;
        if (n > 0)
        {
            translateTimeField (result, n, NEEDS_TRANS("1 hr"), NEEDS_TRANS("2 hrs"));
            ++fieldsShown;
        }

        if (fieldsShown < 2)
        {
            n = std::abs ((int) inMinutes()) % 60;
            if (n > 0)
            {
                translateTimeField (result, n, NEEDS_TRANS("1 min"), NEEDS_TRANS("2 mins"));
                ++fieldsShown;
            }

            if (fieldsShown < 2)
            {
                n = std::abs ((int) inSeconds()) % 60;
                if (n > 0)
                {
                    translateTimeField (result, n, NEEDS_TRANS("1 sec"), NEEDS_TRANS("2 secs"));
                    ++fieldsShown;
                }

                if (fieldsShown == 0)
                {
                    n = std::abs ((int) inMilliseconds()) % 1000;
                    if (n > 0)
                        result << n << ' ' << TRANS ("ms");
                }
            }
        }
    }

    return result.trimEnd();
}

}