/** @file colti_integer_value.h
* Contains typedefs representing string values in Colt.
*/

#ifndef HG_COLT_STRING_VALUE
#define HG_COLT_STRING_VALUE

#include "structs/struct_string.h"

/// @brief Pointer to a constant string
typedef const char* ColtLString_t;
/// @brief The type-id of a Colt literal string
#define COLTI_LSTRING_ID 11
/// @brief A string representing a Colt literal string
static const char ColtLString_str[] = "lstring";

#endif //HG_COLT_INTEGER_VALUE