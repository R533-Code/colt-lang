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

/// @brief Represents validity of built-in conversions, and possible warnings to output
typedef enum
{
	/// @brief Represents a valid conversion
	CONV_VALID = 0,
	/// @brief Represents an invalid conversion, which causes an error
	CONV_INVALID = 1,
	/// @brief Represents a warned signed/unsigned conversion
	CONV_WSIGN = 2,
	/// @brief Represents a warned lossy-conversion
	CONV_WLOSSY = 4,
} TypeConversion;

/// @brief Represents a type's informations, which is a name and an ID
typedef struct
{
	/// @brief The name of the Type
	StringView name;
	/// @brief The ID of the Type
	uint64_t type_id;
	/// @brief The size in memory of the Type
	uint64_t byte_size;
	/// @brief Array of possible built-in conversions
	const TypeConversion* valid_conversions;
	/// @brief The alignment of the Type
	uint64_t alignment;
} TypeInfo;

/// @brief Represents a type, by storing its informations and a 'const' flag
typedef struct
{
	/// @brief The informations about the Type
	const TypeInfo* typeinfo;
	/// @brief If true, than the type is marked 'const'
	bool is_const;
} Type;

/// @brief Represents the possible built-in conversions of ColtVoid_t
static const TypeConversion ColtVoidConvTo[14] = {
	CONV_INVALID, //void
	CONV_INVALID, //bool
	CONV_INVALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //unsigned int
	CONV_INVALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //signed int
	CONV_INVALID, CONV_INVALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtBool_t
static const TypeConversion ColtBoolConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_VALID, CONV_VALID, CONV_INVALID, CONV_INVALID, //unsigned int
	CONV_VALID, CONV_VALID, CONV_INVALID, CONV_INVALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtU8_t
static const TypeConversion ColtU8ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_VALID, CONV_VALID, CONV_VALID, CONV_VALID, //unsigned int
	CONV_WSIGN, CONV_VALID, CONV_VALID, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtU16_t
static const TypeConversion ColtU16ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY, CONV_VALID, CONV_VALID, CONV_VALID, //unsigned int
	CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, CONV_VALID, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtU32_t
static const TypeConversion ColtU32ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY, CONV_WLOSSY, CONV_VALID, CONV_VALID, //unsigned int
	CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtU64_t
static const TypeConversion ColtU64ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_VALID, //unsigned int
	CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtI8_t
static const TypeConversion ColtI8ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WSIGN, CONV_WSIGN, CONV_WSIGN, CONV_WSIGN, //unsigned int
	CONV_VALID, CONV_VALID, CONV_VALID, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtI16_t
static const TypeConversion ColtI16ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, CONV_WSIGN, CONV_WSIGN, //unsigned int
	CONV_WLOSSY, CONV_VALID, CONV_VALID, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtI32_t
static const TypeConversion ColtI32ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, CONV_WSIGN, //unsigned int
	CONV_WLOSSY, CONV_WLOSSY, CONV_VALID, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtI64_t
static const TypeConversion ColtI64ConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WLOSSY | CONV_WSIGN, CONV_WSIGN, //unsigned int
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_VALID, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtFloat_t
static const TypeConversion ColtFloatConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, //unsigned int
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, //signed int
	CONV_VALID, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtDouble_t
static const TypeConversion ColtDoubleConvTo[14] = {
	CONV_INVALID, //void
	CONV_VALID, //bool
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, //unsigned int
	CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, CONV_WLOSSY, //signed int
	CONV_WLOSSY, CONV_VALID,	//floating points
	CONV_INVALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtDouble_t
static const TypeConversion ColtLStringConvTo[14] = {
	CONV_INVALID, //void
	CONV_INVALID, //bool
	CONV_INVALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //unsigned int
	CONV_INVALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //signed int
	CONV_INVALID, CONV_INVALID,	//floating points
	CONV_VALID, CONV_INVALID //lstring & char
};

/// @brief Represents the possible built-in conversions of ColtDouble_t
static const TypeConversion ColtCharConvTo[14] = {
	CONV_INVALID, //void
	CONV_INVALID, //bool
	CONV_VALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //unsigned int
	CONV_VALID, CONV_INVALID, CONV_INVALID, CONV_INVALID, //signed int
	CONV_INVALID, CONV_INVALID,	//floating points
	CONV_INVALID, CONV_VALID //lstring & char
};

/// @brief Type representing a built-in single-byte character
static const TypeInfo ColtChar = {
	.name.start = ColtChar_str,
	.name.end = ColtChar_str + 4,
	.type_id = ID_COLT_CHAR,
	.byte_size = sizeof(ColtChar_t),
	.valid_conversions = ColtCharConvTo,
	.alignment = _Alignof(ColtChar_t)
};

/// @brief Type representing a built-in bool
static const TypeInfo ColtBool = {
	.name.start = ColtBool_str,
	.name.end = ColtBool_str + 4,
	.type_id = ID_COLT_BOOL,
	.byte_size = sizeof(ColtBool_t),
	.valid_conversions = ColtBoolConvTo,
	.alignment = _Alignof(ColtBool_t)
};

/// @brief Type representing a built-in unsigned 8-bit integer
static const TypeInfo ColtU8 = {
	.name.start = ColtU8_str,
	.name.end = ColtU8_str + 3,
	.type_id = ID_COLT_U8,
	.byte_size = sizeof(ColtU8_t),
	.valid_conversions = ColtU8ConvTo,
	.alignment = _Alignof(ColtU8_t)
};

/// @brief Type representing a built-in unsigned 16-bit integer
static const TypeInfo ColtU16 = {
	.name.start = ColtU16_str,
	.name.end = ColtU16_str + 3,
	.type_id = ID_COLT_U16,
	.byte_size = sizeof(ColtU16_t),
	.valid_conversions = ColtU16ConvTo,
	.alignment = _Alignof(ColtU16_t)
};

/// @brief Type representing a built-in unsigned 32-bit integer
static const TypeInfo ColtU32 = {
	.name.start = ColtU32_str,
	.name.end = ColtU32_str + 3,
	.type_id = ID_COLT_U32,
	.byte_size = sizeof(ColtU32_t),
	.valid_conversions = ColtU32ConvTo,
	.alignment = _Alignof(ColtU32_t)
};

/// @brief Type representing a built-in unsigned 64-bit integer
static const TypeInfo ColtU64 = {
	.name.start = ColtU64_str,
	.name.end = ColtU64_str + 3,
	.type_id = ID_COLT_U64,
	.byte_size = sizeof(ColtU64_t),
	.valid_conversions = ColtU64ConvTo,
	.alignment = _Alignof(ColtU64_t)
};

/// @brief Type representing a built-in signed 8-bit integer
static const TypeInfo ColtI8 = {
	.name.start = ColtI8_str,
	.name.end = ColtI8_str + 2,
	.type_id = ID_COLT_I8,
	.byte_size = sizeof(ColtI8_t),
	.valid_conversions = ColtI8ConvTo,
	.alignment = _Alignof(ColtI8_t)
};

/// @brief Type representing a built-in signed 16-bit integer
static const TypeInfo ColtI16 = {
	.name.start = ColtI16_str,
	.name.end = ColtI16_str + 3,
	.type_id = ID_COLT_I16,
	.byte_size = sizeof(ColtI16_t),
	.valid_conversions = ColtI16ConvTo,
	.alignment = _Alignof(ColtI16_t)
};

/// @brief Type representing a built-in signed 32-bit integer
static const TypeInfo ColtI32 = {
	.name.start = ColtI32_str,
	.name.end = ColtI32_str + 3,
	.type_id = ID_COLT_I32,
	.byte_size = sizeof(ColtI32_t),
	.valid_conversions = ColtI32ConvTo,
	.alignment = _Alignof(ColtI32_t)
};

/// @brief Type representing a built-in signed 64-bit integer
static const TypeInfo ColtI64 = {
	.name.start = ColtI64_str,
	.name.end = ColtI64_str + 3,
	.type_id = ID_COLT_I64,
	.byte_size = sizeof(ColtI64_t),
	.valid_conversions = ColtI64ConvTo,
	.alignment = _Alignof(ColtI64_t)
};

/// @brief Type representing a built-in float
static const TypeInfo ColtFloat = {
	.name.start = ColtFloat_str,
	.name.end = ColtFloat_str + 5,
	.type_id = ID_COLT_FLOAT,
	.byte_size = sizeof(ColtFloat_t),
	.valid_conversions = ColtFloatConvTo,
	.alignment = _Alignof(ColtFloat_t)
};

/// @brief Type representing a built-in double
static const TypeInfo ColtDouble = {
	.name.start = ColtDouble_str,
	.name.end = ColtDouble_str + 6,
	.type_id = ID_COLT_DOUBLE,
	.byte_size = sizeof(ColtDouble_t),
	.valid_conversions = ColtDoubleConvTo,
	.alignment = _Alignof(ColtDouble_t)
};

/// @brief Type representing a built-in literal
static const TypeInfo ColtLString = {
	.name.start = ColtLString_str,
	.name.end = ColtLString_str + 7,
	.type_id = ID_COLT_LSTRING,
	.byte_size = sizeof(ColtLString_t),
	.valid_conversions = ColtLStringConvTo,
	.alignment = _Alignof(ColtLString_t)
};

/// @brief Type representing the absence of type
static const TypeInfo ColtVoid = {
	.name.start = ColtVoid_str,
	.name.end = ColtVoid_str + 4,
	.type_id = ID_COLT_VOID,
	.byte_size = 0,
	.valid_conversions = ColtVoidConvTo,
	.alignment = 0
};

/// @brief Check if two types are equal (regardless of their sign-ess)
/// @param lhs The left hand side
/// @param rhs The right hand side
/// @return True if both type's true ID are the same
bool is_type_equal(Type lhs, Type rhs);

/// @brief Check if a type is greater than another (regardless of their sign-ess)
/// @param lhs The left hand side
/// @param rhs The right hand side
/// @return True if the first type's true ID is greater than the second's
bool is_type_greater(Type lhs, Type rhs);

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

/// @brief Returns the best suitable built-in type for 2 types
/// @param lhs The first type
/// @param rhs The second type
/// @return The intersection of the types
Type builtin_inter_type(Type lhs, Type rhs);

#endif //HG_COLT_TYPE