/** @file colti_integer_value.h
* Contains typedefs representing integers and bool values in Colt.
*/

#ifndef HG_COLTI_INTEGER_VALUE
#define HG_COLTI_INTEGER_VALUE

#include <stdint.h>
#include <stdbool.h>

/// @brief Byte representing a 2 state value 'true' (1) 'false' (0)
typedef bool ColtiBool;
/// @brief The type-id of a Colt bool
#define COLTI_BOOL_ID 0
/// @brief A string representing a Colt bool
static const char ColtBool_str[] = "bool";

/// @brief Unsigned 8 bit integer
typedef uint8_t ColtiU8;
/// @brief The type-id of a Colt u8
#define COLTI_U8_ID 1
/// @brief A string representing a Colt unsigned 8-bit integer
static const char ColtU8_str[] = "u8";

/// @brief Unsigned 16 bit integer
typedef uint16_t ColtiU16;
/// @brief The type-id of a Colt u16
#define COLTI_U16_ID 2
/// @brief A string representing a Colt unsigned 16-bit integer
static const char ColtU16_str[] = "u16";

/// @brief Unsigned 32 bit integer
typedef uint32_t ColtiU32;
/// @brief The type-id of a Colt u32
#define COLTI_U32_ID 3
/// @brief A string representing a Colt unsigned 32-bit integer
static const char ColtU32_str[] = "u32";

/// @brief Unsigned 64 bit integer
typedef uint64_t ColtiU64;
/// @brief The type-id of a Colt u64
#define COLTI_U64_ID 4
/// @brief A string representing a Colt unsigned 64-bit integer
static const char ColtU64_str[] = "u64";

/// @brief Signed 8 bit integer
typedef int8_t ColtiI8;
/// @brief The type-id of a Colt i8
#define COLTI_I8_ID 5
/// @brief A string representing a Colt signed 8-bit integer
static const char ColtI8_str[] = "i8";

/// @brief Signed 16 bit integer
typedef int16_t ColtiI16;
/// @brief The type-id of a Colt i16
#define COLTI_I16_ID 6
/// @brief A string representing a Colt signed 16-bit integer
static const char ColtI16_str[] = "i16";

/// @brief Signed 32 bit integer
typedef int32_t ColtiI32;
/// @brief The type-id of a Colt i32
#define COLTI_I32_ID 7
/// @brief A string representing a Colt signed 32-bit integer
static const char ColtI32_str[] = "i32";

/// @brief Signed 64 bit integer
typedef int64_t ColtiI64;
/// @brief The type-id of a Colt i64
#define COLTI_I64_ID 8
/// @brief A string representing a Colt signed 64-bit integer
static const char ColtI64_str[] = "i64";

#endif //HG_COLTI_INTEGER_VALUE