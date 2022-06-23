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

#ifndef HG_COLT_AST
#define HG_COLT_AST

#include "common.h"
#include "token.h"
#include "scanner.h"
#include "structs/struct_table.h"
#include "scan_option.h"
#include "expr.h"

/// @brief Represents an Abstract Syntax Tree of a parsed string.
/// The AST can be used with a code generator. This allows
/// to add a VM, or trans-pile Colt code to another language.
typedef struct
{
	/// @brief The AST
	ExprArray expr;
	/// @brief The scanner providing the lexemes for the AST
	Scanner scan;
	/// @brief The number of errors encountered
	uint16_t error_nb;
	/// @brief The number of warnings encountered
	uint16_t warning_nb;
	/// @brief Options used when parsing
	const ColtScanOptions* options;
	/// @brief The next token to consume from the scanner
	Token current_tkn;
	/// @brief Table for string literals and const/global variables
	ASTTable table;
} AST;

/// @brief Initializes an AST
/// @param ast The AST to initialize
void ASTInit(AST* ast);

/// @brief Frees any resources used by the AST
/// @param ast The AST to modify
void ASTFree(AST* ast);

/// @brief Parses the string stored in the initialized AST
/// @param ast The AST to modify
/// @param to_parse The StringView to parse
/// @param options The option used when scanning
/// @return True if no error were found
bool ASTParse(AST* ast, StringView to_parse, const ColtScanOptions* options);

/// @brief Reset the whole AST (its 'var_table' included) to its initialized state
/// @param ast The AST to modify
void ASTReset(AST* ast);

/************************************
IMPLEMENTATION HELPERS
************************************/

/// @brief Prints a warning
/// @param ast The AST from which to extract the current line and lexeme
/// @param line_nb The line number
/// @param line A StringView over the line
/// @param lexeme A StringView over the lexeme
/// @param format The warning (printf format)
/// @param ... The arguments to format
void ast_gen_warning(AST* ast, uint64_t line_nb, StringView line, StringView lexeme, const char* format, ...);

/// @brief Prints an error and enters panic mode
/// @param ast The AST from which to extract the current line and lexeme
/// @param line_nb The line number
/// @param line A StringView over the line
/// @param lexeme A StringView over the lexeme
/// @param format The error (printf format)
/// @param ... The arguments to format
void ast_gen_error(AST* ast, uint64_t line_nb, StringView line, StringView lexeme, const char* format, ...);

/// @brief Consumes token until an TKN_EOF/TKN_SEMICOLON/TKN_RIGHT_PAREN/TKN_RIGHT_CURLY is hit, and update error counter
/// @param ast The AST whose tokens to consume
void ast_enter_panic_mode(AST* ast);

/// @brief Converts two expressions so that their types match
/// @param lhs A pointer to the left hand side
/// @param rhs A pointer to the right hand side
void ast_handle_conversion(Expr** lhs, Expr** rhs);

/// @brief Does the type checking and error handling of built-in operators.
/// This function returns the return type of built-in operators, handling errors
/// when operands are not valid for the operator.
/// This function will return ColtBool on error, but while this might seem counter-intuitive,
/// as it generates an error, the AST is marked as invalid, meaning the type won't be read.
/// @param ast The AST from which to generate errors
/// @param lhs The left hand side of the operator
/// @param binary_op The token representing the operator
/// @param rhs The right hand side of the operator
/// @param line_nb The line number, for when an error is generated
/// @param line A StringView of the line, for when error is generated
/// @param lexeme A StringView of the lexeme, for when error is generated
/// @return The return type of the operator
Type ast_operator_return_type(AST* ast, Type lhs, Token binary_op, Type rhs, uint64_t line_nb, StringView line, StringView lexeme);

/// @brief Returns the precedence of an operator, and prints an error if the token is not an operator.
/// Returns 100 if the token is not an operator. While other values could have been chosen,
/// to cut down error handling code size, a value greater than 13 was needed.
/// @param ast The AST use to print an error
/// @param token The token to get the precedence of
/// @return A value between 0-13 (with 13 being the highest precedence) or 100 if an error has been detected
int impl_op_precedence(AST* ast, Token token);

/// @brief Parses a binary expression or a primary expression.
/// @param ast The AST from which to parse
/// @param op_precedence The current_tkn operator precedence (which when first call should be -1)
/// @return An Expr* representing either a binary or primary expression
Expr* impl_binary_expr(AST* ast, int op_precedence);

/// @brief Parses a primary expression (which can be a UnaryExpr, LiteralExpr or (...)).
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

/// @brief An expression is either a variable declaration or a binary expression
/// @param ast The AST from which to parse
/// @return An Expr* representing the parsed expression or NULL for errors
Expr* impl_expression(AST* ast);

/// @brief Parses a variable declaration, and adds the variable to the 'var_table'
/// @param ast The AST from which to parse
/// @return An Expr* representing the parsed expression or NULL for errors
Expr* impl_variable_declaration(AST* ast);

/// @brief Returns true if the token represents an assignment
/// @param tkn The token to check for
/// @return True if the token is an assignment
bool is_assignment_token(Token tkn);

/// @brief Returns true if the expression is a binary expression representing an assignment
/// @param expr The expression to check for
/// @return True if the expression is binary having an assignment token
bool is_assignment_expr(Expr* expr);

#endif //HG_COLT_AST
