/** @file colt_integer_value.h
* Contains typedefs representing integers and bool values in Colt.
*/

#ifndef HG_COLT_INTEGER_VALUE
#define HG_COLT_INTEGER_VALUE

#include <stdint.h>
#include <stdbool.h>

/// @brief One byte character
typedef char ColtChar_t;
/// @brief The type-id of a Colt char
#define COLTI_CHAR_ID 0
/// @brief A string representing a Colt char string
static const char ColtChar_str[] = "char";

/// @brief Byte representing a 2 state value 'true' (1) 'false' (0)
typedef bool ColtBool_t;
/// @brief The type-id of a Colt bool
#define COLTI_BOOL_ID 1
/// @brief A string representing a Colt bool
static const char ColtBool_str[] = "bool";

/// @brief Unsigned 8 bit integer
typedef uint8_t ColtU8_t;
/// @brief The type-id of a Colt u8
#define COLTI_U8_ID 2
/// @brief A string representing a Colt unsigned 8-bit integer
static const char ColtU8_str[] = "u8";

/// @brief Unsigned 16 bit integer
typedef uint16_t ColtU16_t;
/// @brief The type-id of a Colt u16
#define COLTI_U16_ID 3
/// @brief A string representing a Colt unsigned 16-bit integer
static const char ColtU16_str[] = "u16";

/// @brief Unsigned 32 bit integer
typedef uint32_t ColtU32_t;
/// @brief The type-id of a Colt u32
#define COLTI_U32_ID 4
/// @brief A string representing a Colt unsigned 32-bit integer
static const char ColtU32_str[] = "u32";

/// @brief Unsigned 64 bit integer
typedef uint64_t ColtU64_t;
/// @brief The type-id of a Colt u64
#define COLTI_U64_ID 5
/// @brief A string representing a Colt unsigned 64-bit integer
static const char ColtU64_str[] = "u64";

/// @brief Signed 8 bit integer
typedef int8_t ColtI8_t;
/// @brief The type-id of a Colt i8
#define COLTI_I8_ID 6
/// @brief A string representing a Colt signed 8-bit integer
static const char ColtI8_str[] = "i8";

/// @brief Signed 16 bit integer
typedef int16_t ColtI16_t;
/// @brief The type-id of a Colt i16
#define COLTI_I16_ID 7
/// @brief A string representing a Colt signed 16-bit integer
static const char ColtI16_str[] = "i16";

/// @brief Signed 32 bit integer
typedef int32_t ColtI32_t;
/// @brief The type-id of a Colt i32
#define COLTI_I32_ID 8
/// @brief A string representing a Colt signed 32-bit integer
static const char ColtI32_str[] = "i32";

/// @brief Signed 64 bit integer
typedef int64_t ColtI64_t;
/// @brief The type-id of a Colt i64
#define COLTI_I64_ID 9
/// @brief A string representing a Colt signed 64-bit integer
static const char ColtI64_str[] = "i64";

/// @brief Absence of type: any expression with no result
typedef void ColtVoid_t;
/// @brief The type-id of a Colt void
#define COLTI_VOID_ID 13
/// @brief A string representing a Colt void
static const char ColtVoid_str[] = "void";

#endif //HG_COLT_INTEGER_VALUE