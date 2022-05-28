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
	StringView name;
	uint64_t type_id;
} Type;

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
Expr* make_literal_expr(QWORD value, OperandType type);

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
/// @return True if the type is a ColtiFloat/ColtiDouble
bool impl_is_type_floating(OperandType type);

#endif //HG_COLTI_EXPR