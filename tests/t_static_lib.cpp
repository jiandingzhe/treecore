#include "TestFramework.h"

#include "treecore/File.h"
#include "treecore/Holder.h"
#include "treecore/JSON.h"
#include "treecore/Object.h"
#include "treecore/Result.h"
#include "treecore/String.h"
#include "treecore/StringRef.h"
#include "treecore/Variant.h"
#include "treecore/ZipFile.h"

using namespace treecore;

const char* json_content = 
    "{"
    "\"a\":1,"
    "\"b\":2"
    "}"
    ;

struct FooBar : public Object
{
    String foo;
    var bar;
};

void TestFramework::content()
{
    {
    Holder<FooBar> foobar_instance = new FooBar();
    OK(foobar_instance);
    }

    {
    var json_data;
    Result json_re = JSON::parse(json_content, json_data);
    OK(json_re);
}

    {
        File f_zip = File::getCurrentWorkingDirectory().getChildFile("zip_input.zip");
        ZipFile zip(f_zip);
    }
}