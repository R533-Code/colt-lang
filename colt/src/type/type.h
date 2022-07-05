/** @file type.h
* Contains a struct representing a Colt Type.
* A Type has a name, an ID and a size in memory.
* For user defined types, more fields are to be required, like memory layout...
* This header also contains const Type representing each built-in type.
*/

#ifndef HG_COLT_TYPE
#define HG_COLT_TYPE

#include "common.h"
#include "type/colt_floating_value.h"
#include "type/colt_integer_value.h"
#include "type/colt_string_value.h"
#include "structs/struct_string.h"

/// @brief Compares a Type to a TypeID
#define TypeEqualTypeID(type, ID) ((type).typeinfo->type_id == (ID))
/// @brief Get the type ID of a Type
#define TypeGetID(type) ((type).typeinfo->type_id)

/// @brief Represents a type, which is a name and an ID
typedef struct
{
	/// @brief The name of the Type
	StringView name;
	/// @brief The ID of the Type
	uint64_t type_id;
	/// @brief The size in memory of the Type
	uint64_t byte_size;
} TypeInfo;

typedef struct
{
	TypeInfo* typeinfo;
	bool is_const;
} Type;


static const TypeInfo ColtVoid		= { .name.start = ColtVoid_str,		.name.end = ColtVoid_str + 4,		.type_id = ID_COLT_VOID,		.byte_size = 0 };
/// @brief Type representing a built-in bool
static const TypeInfo ColtBool		= { .name.start = ColtBool_str,		.name.end = ColtBool_str + 4,		.type_id = ID_COLT_BOOL,		.byte_size = sizeof(ColtBool_t) };
/// @brief Type representing a built-in unsigned 8-bit integer
static const TypeInfo ColtU8		= { .name.start = ColtU8_str,		.name.end = ColtU8_str + 3,			.type_id = ID_COLT_U8,			.byte_size = sizeof(ColtU8_t) };
/// @brief Type representing a built-in unsigned 16-bit integer
static const TypeInfo ColtU16		= { .name.start = ColtU16_str,		.name.end = ColtU16_str + 3,		.type_id = ID_COLT_U16,			.byte_size = sizeof(ColtU16_t) };
/// @brief Type representing a built-in unsigned 32-bit integer
static const TypeInfo ColtU32		= { .name.start = ColtU32_str,		.name.end = ColtU32_str + 3,		.type_id = ID_COLT_U32,			.byte_size = sizeof(ColtU32_t) };
/// @brief Type representing a built-in unsigned 64-bit integer
static const TypeInfo ColtU64		= { .name.start = ColtU64_str,		.name.end = ColtU64_str + 3,		.type_id = ID_COLT_U64,			.byte_size = sizeof(ColtU64_t) };
/// @brief Type representing a built-in signed 8-bit integer
static const TypeInfo ColtI8		= { .name.start = ColtI8_str,		.name.end = ColtI8_str + 2,			.type_id = ID_COLT_I8,			.byte_size = sizeof(ColtI8_t) };
/// @brief Type representing a built-in signed 16-bit integer
static const TypeInfo ColtI16		= { .name.start = ColtI16_str,		.name.end = ColtI16_str + 3,		.type_id = ID_COLT_I16,			.byte_size = sizeof(ColtI16_t) };
/// @brief Type representing a built-in signed 32-bit integer
static const TypeInfo ColtI32		= { .name.start = ColtI32_str,		.name.end = ColtI32_str + 3,		.type_id = ID_COLT_I32,			.byte_size = sizeof(ColtI32_t) };
/// @brief Type representing a built-in signed 64-bit integer
static const TypeInfo ColtI64		= { .name.start = ColtI64_str,		.name.end = ColtI64_str + 3,		.type_id = ID_COLT_I64,			.byte_size = sizeof(ColtI64_t) };
/// @brief Type representing a built-in float
static const TypeInfo ColtFloat		= { .name.start = ColtFloat_str,	.name.end = ColtFloat_str + 5,		.type_id = ID_COLT_FLOAT,		.byte_size = sizeof(ColtFloat_t) };
/// @brief Type representing a built-in double
static const TypeInfo ColtDouble	= { .name.start = ColtDouble_str,	.name.end = ColtDouble_str + 6,		.type_id = ID_COLT_DOUBLE,		.byte_size = sizeof(ColtDouble_t) };
/// @brief Type representing a built-in literal
static const TypeInfo ColtLString	= { .name.start = ColtLString_str,  .name.end = ColtLString_str + 7,	.type_id = ID_COLT_LSTRING,		.byte_size = sizeof(ColtLString_t) };

/// @brief Check for if a type is built-in
/// @param type The type to check for
/// @return True if the type is built-in
bool is_type_builtin(Type type);

/// @brief Check if a type is a built-in signed integer
/// @param type The type_id to check for
/// @return True if the type is a signed integer
bool is_type_signed_int(Type type);

/// @brief Check if a type is a built-in unsigned integer
/// @param type The type_id to check for
/// @return True if the type is an unsigned integer
bool is_type_unsigned_int(Type type);

/// @brief Checks if a type is a built-in integer regardless of its sign
/// @param type The type_id to check for
/// @return True if the type is an unsigned/signed integer
bool is_type_integral(Type type);

/// @brief Check if a type is a built-in floating point type
/// @param type The type_id to check for
/// @return True if the type is a ColtFloat_t/ColtDouble_t
bool is_type_floating(Type type);

/// @brief Returns the sign equivalent Type of an unsigned type ID
/// @param type The unsigned type ID
/// @return The signed type equivalent
Type type_unsigned_to_signed(Type type);

/// @brief Returns the type from a BuiltinTypeID
/// @param id The ID to representing a buit-in type
/// @return A Type corresponding to the ID
TypeInfo* TypeInfoGetBuiltInFromID(BuiltinTypeID id);

#endif //HG_COLT_TYPE