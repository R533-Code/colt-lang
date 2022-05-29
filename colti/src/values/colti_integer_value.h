/** @file colti_integer_value.h
* Contains typedefs representing integers and bool values in Colt.
*/

#ifndef HG_COLTI_INTEGER_VALUE
#define HG_COLTI_INTEGER_VALUE

#include <stdint.h>
#include <stdbool.h>

/// @brief Byte representing a 2 state value 'true' (1) 'false' (0)
typedef bool ColtiBool;
/// @brief The operand representing a bool
#define COLTI_BOOL_ID 0

/// @brief Unsigned 8 bit integer
typedef uint8_t ColtiU8;
/// @brief The operand representing a unsigned 8-bit integer
#define COLTI_U8_ID 1

/// @brief Unsigned 16 bit integer
typedef uint16_t ColtiU16;
/// @brief The operand representing a unsigned 16-bit integer
#define COLTI_U16_ID 2

/// @brief Unsigned 32 bit integer
typedef uint32_t ColtiU32;
/// @brief The operand representing a unsigned 32-bit integer
#define COLTI_U32_ID 3

/// @brief Unsigned 64 bit integer
typedef uint64_t ColtiU64;
/// @brief The operand representing a unsigned 64-bit integer
#define COLTI_U64_ID 4


/// @brief Signed 8 bit integer
typedef int8_t ColtiI8;
/// @brief The operand representing a signed 8-bit integer
#define COLTI_I8_ID 5

/// @brief Signed 16 bit integer
typedef int16_t ColtiI16;
/// @brief The operand representing a signed 16-bit integer
#define COLTI_I16_ID 6

/// @brief Signed 32 bit integer
typedef int32_t ColtiI32;
/// @brief The operand representing a signed 32-bit integer
#define COLTI_I32_ID 7

/// @brief Signed 64 bit integer
typedef int64_t ColtiI64;
/// @brief The operand representing a signed 64-bit integer
#define COLTI_I64_ID 8


#endif //HG_COLTI_INTEGER_VALUE