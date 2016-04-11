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

#include "treecore/InputStream.h"
#include "treecore/MacAddress.h"
#include "treecore/Process.h"
#include "treecore/WebInputStream.h"

#include <ifaddrs.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

namespace treecore
{

void MacAddress::findAllAddresses (Array<MacAddress>& result)
{
    const int s = socket (AF_INET, SOCK_DGRAM, 0);
    if (s != -1)
    {
        struct ifaddrs* addrs = nullptr;

        if (getifaddrs (&addrs) != -1)
        {
            for (struct ifaddrs* i = addrs; i != nullptr; i = i->ifa_next)
            {
                struct ifreq ifr;
                strcpy (ifr.ifr_name, i->ifa_name);
                ifr.ifr_addr.sa_family = AF_INET;

                if (ioctl (s, SIOCGIFHWADDR, &ifr) == 0)
                {
                    MacAddress ma ((const uint8*) ifr.ifr_hwaddr.sa_data);

                    if (! ma.isNull())
                        result.addIfNotAlreadyThere (ma);
                }
            }

            freeifaddrs (addrs);
        }

        close (s);
    }
}


bool TREECORE_STDCALL Process::openEmailWithAttachments (const String& /* targetEmailAddress */,
                                                      const String& /* emailSubject */,
                                                      const String& /* bodyText */,
                                                      const StringArray& /* filesToAttach */)
{
    treecore_assert_false;    // xxx todo
    return false;
}


} // namespace treecore
