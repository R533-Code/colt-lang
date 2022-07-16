/** @file colti_string_value.h
* Contains typedefs representing string values in Colt.
*/

#ifndef HG_COLT_STRING_VALUE
#define HG_COLT_STRING_VALUE

#include "structs/struct_string.h"

/// @brief Pointer to a constant literal string
typedef const char* ColtLString_t;
/// @brief The type-id of a Colt literal string
#define COLTI_LSTRING_ID 12
/// @brief A string representing a Colt literal string
static const char ColtLString_str[] = "lstring";

/// @brief One byte character
typedef char ColtChar_t;
/// @brief The type-id of a Colt char
#define COLTI_CHAR_ID 13
/// @brief A string representing a Colt char string
static const char ColtChar_str[] = "char";

#endif //HG_COLT_INTEGER_VALUE