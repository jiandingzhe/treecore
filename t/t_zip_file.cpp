#include "treecore/TestFramework.h"

#include "treecore/File.h"
#include "treecore/InputStream.h"
#include "treecore/ZipFile.h"

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
    ZipFile* archive = new ZipFile(File("zip_input.zip"));

    OK(archive);
    IS(archive->getNumEntries(), 3);

    char buffer[512];
    {
        const ZipFile::ZipEntry* entry = archive->getEntry("foo");
        InputStream* stream = archive->createStreamForEntry(*entry);
        int64 len = stream->getTotalLength();
        IS(len, 4);
        IS(stream->read(buffer, 512), len);
        IS(buffer[0], 'f');
        IS(buffer[1], 'o');
        IS(buffer[2], 'o');
        IS(buffer[3], '\n');
    }

    {
        const ZipFile::ZipEntry* entry = archive->getEntry("bar");
        InputStream* stream = archive->createStreamForEntry(*entry);
        int64 len = stream->getTotalLength();
        IS(len, 4);
        IS(stream->read(buffer, 512), len);
        IS(buffer[0], 'b');
        IS(buffer[1], 'a');
        IS(buffer[2], 'r');
        IS(buffer[3], '\n');
    }

    {
        const ZipFile::ZipEntry* entry = archive->getEntry("baz");
        InputStream* stream = archive->createStreamForEntry(*entry);
        int64 len = stream->getTotalLength();
        IS(len, 4);
        IS(stream->read(buffer, 512), len);
        IS(buffer[0], 'b');
        IS(buffer[1], 'a');
        IS(buffer[2], 'z');
        IS(buffer[3], '\n');
    }
}
