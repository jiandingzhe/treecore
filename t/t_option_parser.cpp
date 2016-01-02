#include "treecore/OptionParser.h"
#include "TestFramework.h"

using namespace treecore;

void TestFramework::content()
{
    Array<int> aaa_values;
    Option opt_a("aaa", 'a', "major", &aaa_values, 0, ValueLimit::Free(), "option aaa", "a b c");

    {
        int argc = 4;
        const char* argv[] = {
            "foobarbaz",
            "--aaa",
            "123",
            "456",
        };

        OptionParser parser;
        parser.add_option(opt_a);
        parser.parse_options(argc, argv);

        IS(aaa_values.size(), 2);
        IS(aaa_values[0], 123);
        IS(aaa_values[1], 456);
        IS(argc, 1);
        OK(String(argv[0]) == "foobarbaz");
    }
}
