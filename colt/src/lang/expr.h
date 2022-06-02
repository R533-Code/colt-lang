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

#ifndef HG_COLT_EXPR
#define HG_COLT_EXPR

#include "common.h"
#include "token.h"
#include "type.h"

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
	EXPR_LITERAL,
	/// @brief ConvertExpr type
	EXPR_CONVERT
} ExprIdentifier;

/// @brief Represents an unspecialized expression.
/// As C doesn't provide inheritance, we add these two members in each
/// expression class. 'identifier' is used to know which type of expression
/// is the current expression. This allows us to cast an expression to its
/// actual type. 'expr_type' is the Colt type of the expression.
typedef struct
{
	/// @brief The line number
	uint64_t line_nb;
	/// @brief The line from which the expression was created
	StringView line;
	/// @brief The lexeme representing the expression
	StringView lexeme;
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
	/// @brief The line number
	uint64_t line_nb;
	/// @brief The line from which the expression was created
	StringView line;
	/// @brief The lexeme representing the expression
	StringView lexeme;
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
	/// @brief The line number
	uint64_t line_nb;
	/// @brief The line from which the expression was created
	StringView line;
	/// @brief The lexeme representing the expression
	StringView lexeme;
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
	/// @brief The line number
	uint64_t line_nb;
	/// @brief The line from which the expression was created
	StringView line;
	/// @brief The lexeme representing the expression
	StringView lexeme;
	/// @brief should be EXPR_LITERAL
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on the type of 'value'
	Type expr_type;
	/// @brief The literal value
	QWORD value;
} LiteralExpr;

/// @brief Represents a conversion expression
typedef struct
{
	/// @brief The line number
	uint64_t line_nb;
	/// @brief The line from which the expression was created
	StringView line;
	/// @brief The lexeme representing the expression
	StringView lexeme;
	/// @brief Allows to cast the expression to the right expression type
	ExprIdentifier identifier;
	/// @brief The expression type, which depends on its content
	Type expr_type;
	/// @brief The expression to be casted
	Expr* child;
} ConvertExpr;

/// @brief Allocates a new literal expression on the heap, initializing it
/// @param value The value of the literal expression
/// @param type The type of the literal expression
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated LiteralExpr
Expr* makeLiteralExpr(QWORD value, Type type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new unary expression on the heap, initializing it
/// @param unary_operator The operator of the unary expression (+, -, !, @, ~)
/// @param child The expression on which the apply the unary operator
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated UnaryExpr
Expr* makeUnaryExpr(Token unary_operator, Expr* child, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new binary expression on the heap initializing it
/// @param lhs The left hand side of the operator
/// @param binary_operator The operator
/// @param rhs The right hand side of the operator
/// @param expr_type The expression type
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated BinaryExpr
Expr* makeBinaryExpr(Expr* lhs, Token binary_operator, Expr* rhs, Type expr_type, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Allocates a new convert expression on the heap, initializing it
/// @param expr The expression to cast
/// @param convert_to The Type to which to cast to
/// @param line_nb The line number from which the expression is extracted
/// @param line A StringView over the line containing the expression
/// @param lexeme A StringView over the lexeme representing the expression
/// @return A pointer to a heap allocated ConvertExpr
Expr* makeConvertExpr(Expr* expr, Type convert_to, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Recursively frees a heap-allocated expression.
/// As an Expr* can be a BinaryExpr, or an expression with child expression,
/// we cannot just free the pointer received from a make...Expr.
/// @param ptr The Expr* to free
void freeExpr(Expr* ptr);

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
Type builtin_inter_type(Type lhs, Type rhs);

/// @brief Check if a type is a built-in signed integer
/// @param type The type_id to check for
/// @return True if the type is a signed integer
bool is_type_int(BuiltinTypeID type);

/// @brief Check if a type is a built-in unsigned integer
/// @param type The type_id to check for
/// @return True if the type is an unsigned integer
bool is_type_uint(BuiltinTypeID type);

/// @brief Checks if a type is a built-in integer regardless of its sign
/// @param type The type_id to check for
/// @return True if the type is an unsigned/signed integer
bool is_type_integral(BuiltinTypeID type);

/// @brief Check if a type is a built-in floating point type
/// @param type The type_id to check for
/// @return True if the type is a ColtFloat_t/ColtDouble_t
bool is_type_floating(BuiltinTypeID type);

#endif //HG_COLT_EXPR