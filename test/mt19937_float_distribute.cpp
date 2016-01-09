#include "treecore/MT19937.h"

using namespace treecore;

int main(int argc, char** argv)
{
    MT19937 prng;


    for (int iter = 0; iter < 1000; iter++)
    {
        int num_low = 0;
        int num_high = 0;
        for (int i = 0; i < 100000; i++)
        {
            double value = prng.next_double_yn();
            if (value >= 0.5)
                num_high++;
            else
                num_low++;
        }

        printf("%d %d\n", num_low, num_high);
    }
}
