/*****************************************************************/ /**
 * @file   compiler_limits.h
 * @brief  Contains assertion macros used in the compiler.
 * These macros are not deactivated on Release: they are usually
 * used for overflow checks.
 * 
 * @author RPC
 * @date   October 2024
 *********************************************************************/
#ifndef HG_COLT_COMPILER_LIMITS
#define HG_COLT_COMPILER_LIMITS

#include <exception>
#include "colt/typedefs.h"

#ifndef COLT_NO_COMPILER_ASSERTS
  /// @brief Assertion that is always checked
  #define compiler_assert_true(MESSAGE, COND, ...)                         \
    clt::details::assert_true_multiple(                                    \
        "COMPILER LIMIT: " MESSAGE,                                        \
        clt::source_location::current() __DETAILS__COLT_TO_ASSERTION(COND) \
            COLT_FOR_EACH(__DETAILS__COLT_TO_ASSERTION, __VA_ARGS__))      \
        __DETAILS__COLT_TO_ASSUME(COND)                                    \
            COLT_FOR_EACH(__DETAILS__COLT_TO_ASSUME, __VA_ARGS__)
#else
  #define compiler_assert_true(MESSAGE, COND, ...) ((void)0)
#endif // !COLT_NO_COMPILER_ASSERTS

namespace clt
{
  /// @brief Exception thrown to exit recursive functions (in the AST and Lexer)
  class ExitRecursionException
    : public std::exception
  {};
}

#endif // !HG_COLT_COMPILER_LIMITS