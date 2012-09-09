/**
 * @file XAssert.h
 * Contains the macros and functions required for X_ASSERT.
 * @author Tim Beyne
 */
#ifndef _X_ASSERT_H_INCLUDE_GUARD
#define _X_ASSERT_H_INCLUDE_GUARD

namespace XAssertion {
    int assert_handler(char const* expr, char const* file, int line);
}

#ifdef _MSC_VER
    #define _XASSERT_HALT() __debugbreak()
#else
    #include <cstdlib>
    #define _XASSERT_HALT() exit(__LINE__)
#endif

#ifdef _X_DEBUGMODE
    #define X_ASSERT(x) ((void)(!(x) && XAssertion::assert_handler(#x, __FILE__, __LINE__)\
            && (_XASSERT_HALT(), 1)))
#else
    #define X_ASSERT(x) ((void)sizeof(x))
#endif

#endif // _X_ASSERT_H_INCLUDE_GUARD

