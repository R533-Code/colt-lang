/** @file expr.h
* Contains skeleton of classes representing AST expressions.
* This header contains the struct Expr, which represents an unspecialized expression
* (as if it were a base class), whose members are in every expression-like struct.
* This allows to pass Expr*, regardless of the actual expression type.
* To be able to cast the Expr* to its actual type, a field 'identifier' (ExprIdentifier)
* contains actual expression type.
* 
* As all expression have a type, as with 'identifier', the 'expr_type' (which contains
* the actual type of the Colt expression) is a field contained in every expression struct.
* This is done to avoid the hassle of converting the Expr* to its actual expression-type,
* as this information is useless when just checking for the type of another expression.
*/

#ifndef HG_COLTI_EXPR
#define HG_COLTI_EXPR

#include "common.h"
#include "token.h"

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
static const Type ColtBool		= { .name.start = ColtBool_str,		.name.end = ColtBool_str + 4,		.type_id = COLTI_BOOL,		.byte_size = sizeof(ColtBool_t) };
/// @brief Type representing a built-in unsigned 8-bit integer
static const Type ColtUInt8		= { .name.start = ColtU8_str,		.name.end = ColtU8_str + 3,			.type_id = COLTI_UINT8,		.byte_size = sizeof(ColtU8_t) };
/// @brief Type representing a built-in unsigned 16-bit integer
static const Type ColtUInt16	= { .name.start = ColtU16_str,		.name.end = ColtU16_str + 3,		.type_id = COLTI_UINT16,	.byte_size = sizeof(ColtU16_t) };
/// @brief Type representing a built-in unsigned 32-bit integer
static const Type ColtUInt32	= { .name.start = ColtU32_str,		.name.end = ColtU32_str + 3,		.type_id = COLTI_UINT32,	.byte_size = sizeof(ColtU32_t) };
/// @brief Type representing a built-in unsigned 64-bit integer
static const Type ColtUInt64	= { .name.start = ColtU64_str,		.name.end = ColtU64_str + 3,		.type_id = COLTI_UINT64,	.byte_size = sizeof(ColtU64_t) };
/// @brief Type representing a built-in signed 8-bit integer
static const Type ColtInt8		= { .name.start = ColtI8_str,		.name.end = ColtI8_str + 2,			.type_id = COLTI_INT8,		.byte_size = sizeof(ColtI8_t) };
/// @brief Type representing a built-in signed 16-bit integer
static const Type ColtInt16		= { .name.start = ColtI16_str,		.name.end = ColtI16_str + 3,		.type_id = COLTI_INT16,		.byte_size = sizeof(ColtI16_t) };
/// @brief Type representing a built-in signed 32-bit integer
static const Type ColtInt32		= { .name.start = ColtI32_str,		.name.end = ColtI32_str + 3,		.type_id = COLTI_INT32,		.byte_size = sizeof(ColtI32_t) };
/// @brief Type representing a built-in signed 64-bit integer
static const Type ColtInt64		= { .name.start = ColtI64_str,		.name.end = ColtI64_str + 3,		.type_id = COLTI_INT64,		.byte_size = sizeof(ColtI64_t) };
/// @brief Type representing a built-in float
static const Type ColtFloat		= { .name.start = ColtFloat_str,	.name.end = ColtFloat_str + 5,		.type_id = COLTI_FLOAT,		.byte_size = sizeof(ColtFloat_t) };
/// @brief Type representing a built-in double
static const Type ColtDouble	= { .name.start = ColtDouble_str,	.name.end = ColtDouble_str + 6,		.type_id = COLTI_DOUBLE,	.byte_size = sizeof(ColtDouble_t) };

/// @brief Represent the type of the expression holden by an Expr*.
/// As C does not have inheritance, every expression class contains
/// an 'identifier' member at the same layout location. By using this
/// information, we can cast that pointer to the right expression struct.
typedef enum
{
	/// @brief Variable expression
	EXPR_VAR,
	/// @brief Function expression
	EXPR_FN,
	/// @brief UnaryExpr type
	EXPR_UNARY,
	/// @brief BinaryExpr type
	EXPR_BINARY,
	/// @brief LiteralExpr type
	EXPR_LITERAL
} ExprIdentifier;

/// @brief Represents an unspecialized expression.
/// As C doesn't provide inheritance, we add these two members in each
/// expression class. 'identifier' is used to know which type of expression
/// is the current expression. This allows us to cast an expression to its
/// actual type. 'expr_type' is the Colt type of the expression.
typedef struct
{
	/// @brief Allows to cast the expression to the right expression type
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;
} Expr;

/// @brief Represents a unary expression.
/// A unary expression is composed of a unary operator (-, ~, +, @, !),
/// and the expression to which the operator is applied
typedef struct
{
	/// @brief should be EXPR_UNARY
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;
	/// @brief The unary operator
	Token expr_operator;
	/// @brief The expression on which the unary operator is applied
	Expr* child;
} UnaryExpr;

/// @brief Represents a binary expression.
/// A binary expression is composed of a binary operator, and both expression
/// on which the operator is applied
typedef struct
{
	/// @brief should be EXPR_BINARY
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;
	/// @brief The unary operator
	Token expr_operator;
	/// @brief The left hand side of the expression
	Expr* lhs;
	/// @brief The right hand side of the expression
	Expr* rhs;
} BinaryExpr;

/// @brief Represents a literal expression.
/// A LiteralExpr can contain literal integer, floating point, string, or boolean
typedef struct
{
	/// @brief should be EXPR_LITERAL
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'value'
	Type expr_type;
	/// @brief The literal value
	QWORD value;
} LiteralExpr;

/// @brief Allocates a new literal expression on the heap, initializing it
/// @param value The value of the literal expression
/// @param type The type of the literal expression
/// @return A pointer to a heap allocated LiteralExpr 
Expr* make_literal_expr(QWORD value, Type type);

/// @brief Allocates a new unary expression on the heap, initializing it
/// @param unary_operator The operator of the unary expression (+, -, !, @, ~)
/// @param child The expression on which the apply the unary operator
/// @return A pointer to a heap allocated UnaryExpr
Expr* make_unary_expr(Token unary_operator, Expr* child);

/// @brief Allocates a new binary expression on the heap initializing it
/// @param lhs The left hand side of the operator
/// @param binary_operator The operator
/// @param rhs The right hand side of the operator
/// @return A pointer to a heap allocated BinaryExpr
Expr* make_binary_expr(Expr* lhs, Token binary_operator, Expr* rhs);

/*******************************
IMPLEMENTATION HELPER
*******************************/

/// @brief Returns the type that should be returned by an operator taking 'lhs' and 'rhs'
/// @param lhs The left hand side of the operator
/// @param binary_operator The token representing the operator
/// @param rhs The right hand side of the operator
/// @return The Type of the operator, or an outputs an error
Type impl_operator_type(Type lhs, Token binary_operator, Type rhs);

/// @brief Returns the best suitable built-in type for 2 types
/// @param lhs The first type
/// @param rhs The second type
/// @return The intersection of the types
Type impl_builtin_inter_type(Type lhs, Type rhs);

/// @brief Check if a type is a built-in signed integer
/// @param type The type_id to check for
/// @return True if the type is a signed integer
bool impl_is_type_int(OperandType type);

/// @brief Check if a type is a built-in unsigned integer
/// @param type The type_id to check for
/// @return True if the type is a unsigned integer
bool impl_is_type_uint(OperandType type);

/// @brief Check if a type is a built-in floating point type
/// @param type The type_id to check for
/// @return True if the type is a ColtFloat_t/ColtDouble_t
bool impl_is_type_floating(OperandType type);

#endif //HG_COLTI_EXPR