/** @file ast.h
* Abstract Syntax Tree for Colt.
* The AST is responsible of producing a valid representation of a program,
* or fail to do so, reporting the errors.
* The AST, as for the Scanner contains a 'current_tkn' field, which is the NEXT
* token to consume.
* The AST uses Pratt's parsing for operator precedence.
*
* The AST contains a Scanner, which it uses to break and extract lexemes from a StringView.
*/

#ifndef HG_COLTI_AST
#define HG_COLTI_AST

#include "common.h"
#include "token.h"
#include "scanner.h"
#include "expr.h"

/// @brief Represents an Abstract Syntax Tree of a parsed string.
/// The AST can be used with a code generator. This allows
/// to add a VM, or trans-pile Colt code to another language.
typedef struct
{
	/// @brief The AST
	Expr* expr;
	/// @brief The scanner providing the lexemes for the AST
	Scanner scan;
	/// @brief Flag which allows
	bool panic_mode;
	/// @brief The number of errors encountered
	uint16_t error_nb;
	/// @brief The next token to consume from the scanner
	Token current_tkn;
} AST;

/// @brief Initializes an AST
/// @param ast The AST to initialize
/// @param to_parse The StringView to parse
void ASTInit(AST* ast, StringView to_parse);

/// @brief Frees any resources used by the AST
/// @param ast The AST to modify
void ASTFree(AST* ast);

/// @brief Parses the string stored in the initialized AST
/// @param ast The AST to modify
/// @return True if no error were found
bool ASTParse(AST* ast);

/************************************
IMPLEMENTATION HELPERS
************************************/

/// @brief Returns the precedence of an operator, and prints an error if the token is not an operator
/// @param ast The AST use to print an error
/// @param token The token to get the precedence of
/// @return A value between 0-13 (with 13 being the highest precedence) or 100 if an error has been detected
int impl_op_precedence(const AST* ast, Token token);

/// @brief Parses a binary expression or a primary expression.
/// FIXME: add valid tokens
/// @param ast The AST from which to parse
/// @param op_precedence The current_tkn operator precedence (which when first call should be 0)
/// @return An Expr* representing either a binary or primary expression
Expr* impl_binary_expr(AST* ast, int op_precedence);

/// @brief Parses a primary expression (which can be a UnaryExpr, LiteralExpr or (...)).
/// FIXME: add valid tokens
/// @param ast The AST from which to parse
/// @return An Expr* representing the parsed expression
Expr* impl_primary_expr(AST* ast);

/// @brief Parses a unary expression
/// @param ast The AST from which to parse
/// @return An Expr* representing the parsed expression
Expr* impl_unary_expr(AST* ast);

/// @brief Parses a parenthesis expression, verifying it is well enclosed
/// @param ast The AST from which to parse
/// @return An Expr* representing the parsed expression
Expr* impl_paren_expr(AST* ast);

#endif //HG_COLTI_AST
