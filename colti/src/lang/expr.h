#ifndef HG_COLTI_EXPR
#define HG_COLTI_EXPR

#include "common.h"

typedef struct
{
	StringView name;
	uint64_t type_id;
} Type;

typedef enum
{
	EXPR_VAR,
	EXPR_FN,
	EXPR_UNARY,
	EXPR_BINARY,
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

#endif //HG_COLTI_EXPR