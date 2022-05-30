#ifndef HG_COLTI_TYPE
#define HG_COLTI_TYPE

#include "common.h"
#include "values/colti_floating_value.h"
#include "values/colti_integer_value.h"
#include "structs/struct_string.h"

/// @brief Represents a type, which is a name and an ID
typedef struct
{
	/// @brief The name of the Type
	StringView name;
	/// @brief The ID of the Type
	uint64_t type_id;
	/// @brief The size in memory of the Type
	uint64_t byte_size;
} Type;


/// @brief Type representing a built-in bool
static const Type ColtBool = { .name.start = ColtBool_str,		.name.end = ColtBool_str + 4,		.type_id = ID_COLT_BOOL,		.byte_size = sizeof(ColtBool_t) };
/// @brief Type representing a built-in unsigned 8-bit integer
static const Type ColtUInt8 = { .name.start = ColtU8_str,		.name.end = ColtU8_str + 3,			.type_id = ID_COLT_U8,			.byte_size = sizeof(ColtU8_t) };
/// @brief Type representing a built-in unsigned 16-bit integer
static const Type ColtUInt16 = { .name.start = ColtU16_str,		.name.end = ColtU16_str + 3,		.type_id = ID_COLT_U16,			.byte_size = sizeof(ColtU16_t) };
/// @brief Type representing a built-in unsigned 32-bit integer
static const Type ColtUInt32 = { .name.start = ColtU32_str,		.name.end = ColtU32_str + 3,		.type_id = ID_COLT_U32,			.byte_size = sizeof(ColtU32_t) };
/// @brief Type representing a built-in unsigned 64-bit integer
static const Type ColtUInt64 = { .name.start = ColtU64_str,		.name.end = ColtU64_str + 3,		.type_id = ID_COLT_U64,			.byte_size = sizeof(ColtU64_t) };
/// @brief Type representing a built-in signed 8-bit integer
static const Type ColtInt8 = { .name.start = ColtI8_str,		.name.end = ColtI8_str + 2,			.type_id = ID_COLT_I8,			.byte_size = sizeof(ColtI8_t) };
/// @brief Type representing a built-in signed 16-bit integer
static const Type ColtInt16 = { .name.start = ColtI16_str,		.name.end = ColtI16_str + 3,		.type_id = ID_COLT_I16,			.byte_size = sizeof(ColtI16_t) };
/// @brief Type representing a built-in signed 32-bit integer
static const Type ColtInt32 = { .name.start = ColtI32_str,		.name.end = ColtI32_str + 3,		.type_id = ID_COLT_I32,			.byte_size = sizeof(ColtI32_t) };
/// @brief Type representing a built-in signed 64-bit integer
static const Type ColtInt64 = { .name.start = ColtI64_str,		.name.end = ColtI64_str + 3,		.type_id = ID_COLT_I64,			.byte_size = sizeof(ColtI64_t) };
/// @brief Type representing a built-in float
static const Type ColtFloat = { .name.start = ColtFloat_str,	.name.end = ColtFloat_str + 5,		.type_id = ID_COLT_FLOAT,		.byte_size = sizeof(ColtFloat_t) };
/// @brief Type representing a built-in double
static const Type ColtDouble = { .name.start = ColtDouble_str,	.name.end = ColtDouble_str + 6,		.type_id = ID_COLT_DOUBLE,		.byte_size = sizeof(ColtDouble_t) };

#endif //HG_COLTI_TYPE