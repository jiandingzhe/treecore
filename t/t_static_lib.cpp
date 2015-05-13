#include "TestFramework.h"

#include "treejuce/File.h"
#include "treejuce/Holder.h"
#include "treejuce/JSON.h"
#include "treejuce/Object.h"
#include "treejuce/Result.h"
#include "treejuce/String.h"
#include "treejuce/StringRef.h"
#include "treejuce/Variant.h"
#include "treejuce/ZipFile.h"

using namespace treejuce;

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