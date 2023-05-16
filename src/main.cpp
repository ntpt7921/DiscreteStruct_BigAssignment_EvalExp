#include "helper.h"
#include <cstdlib>
#include <string>

#define TEST_ARITH
#undef TEST_LOGIC

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        std::string fileName = argv[i];
#ifdef TEST_ARITH
        /* helper::convertArithmeticExpFromFile(fileName); */
        helper::evaluateArithmeticExpFromFile(fileName);
#endif
#ifdef TEST_LOGIC
        helper::readLogicalExp(fileName);
#endif
    }

    return EXIT_SUCCESS;
}
