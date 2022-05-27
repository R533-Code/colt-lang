/** @file colti_integer_value.h
* Contains typedefs representing integers and bool values in Colt.
*/

#ifndef HG_COLTI_INTEGER_VALUE
#define HG_COLTI_INTEGER_VALUE

#include <stdint.h>
#include <stdbool.h>

/// @brief Byte representing a 2 state value 'true' (1) 'false' (0)
using ColtiBool = bool;
/// @brief The operand representing a bool
#define OPERAND_COLTI_BOOL 0

/// @brief Unsigned 8 bit integer
using ColtiUI8 = uint8_t;
/// @brief The operand representing a unsigned 8-bit integer
#define OPERAND_COLTI_UI8 1

/// @brief Unsigned 16 bit integer
using ColtiUI16 = uint16_t;
/// @brief The operand representing a unsigned 16-bit integer
#define OPERAND_COLTI_UI16 2

/// @brief Unsigned 32 bit integer
using ColtiUI32 = uint32_t;
/// @brief The operand representing a unsigned 32-bit integer
#define OPERAND_COLTI_UI32 3

/// @brief Unsigned 64 bit integer
using ColtiUI64 = uint64_t;
/// @brief The operand representing a unsigned 64-bit integer
#define OPERAND_COLTI_UI64 4


/// @brief Signed 8 bit integer
using ColtiI8 = int8_t;
/// @brief The operand representing a signed 8-bit integer
#define OPERAND_COLTI_I8 5

/// @brief Signed 16 bit integer
using ColtiI16 = int16_t;
/// @brief The operand representing a signed 16-bit integer
#define OPERAND_COLTI_I16 6

/// @brief Signed 32 bit integer
using ColtiI32 = int32_t;
/// @brief The operand representing a signed 32-bit integer
#define OPERAND_COLTI_I32 7

/// @brief Signed 64 bit integer
using ColtiI64 = int64_t;
/// @brief The operand representing a signed 64-bit integer
#define OPERAND_COLTI_I64 8


#endif //HG_COLTI_INTEGER_VALUE