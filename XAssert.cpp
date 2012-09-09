/**
 * @file XAssert.h
 * Implements the assert_handler function found in XAssert.h.
 * @author Tim Beyne
 */
#include "XAssert.h"

#include <iostream>

int XAssertion::assert_handler(char const* expr, char const* file, int line)
{
    std::cerr << "X assertion failed: (fatal)\n"
              << "X_ASSERT(" << expr << ") in " << file << " at line " << line << ".\n"
              << std::endl;
    return 1;
}

