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
	/// @brief The current token from the scanner
	Token current;
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

void impl_expr_free(Expr* expr);

int impl_op_precedence(const AST* ast, Token token);

Expr* impl_binary_expr(AST* ast, int op_precedence);

Expr* impl_primary_expr(AST* ast);

#endif //HG_COLTI_AST
