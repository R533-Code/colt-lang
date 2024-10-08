/*****************************************************************//**
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

#include "colt/macro/assert.h"

#ifndef COLT_NO_COMPILER_ASSERTS
  /// @brief Assertion that is always checked
  #define compiler_assert_true(MESSAGE, COND, ...)                                  \
    clt::details::assert_true_multiple(                                             \
        MESSAGE, clt::source_location::current() __DETAILS__COLT_TO_ASSERTION(COND) \
                     COLT_FOR_EACH(__DETAILS__COLT_TO_ASSERTION, __VA_ARGS__))      \
        __DETAILS__COLT_TO_ASSUME(COND)                                             \
            COLT_FOR_EACH(__DETAILS__COLT_TO_ASSUME, __VA_ARGS__)
#else
  #define compiler_assert_true(MESSAGE, COND, ...) ((void)0)
#endif // !COLT_NO_COMPILER_ASSERTS

#endif // !HG_COLT_COMPILER_LIMITS