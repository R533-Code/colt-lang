#ifndef HG_COLTI_AST
#define HG_COLTI_AST

#include "common.h"
#include "token.h"
#include "scanner.h"
#include "expr.h"

typedef struct
{
	/// @brief The AST
	Expr* expr;
	/// @brief The scanner providing the lexemes for the AST
	Scanner scan;
} AST;

void ASTInit(AST* ast, StringView to_parse);

void ASTFree(AST* ast);

/************************************
IMPLEMENTATION HELPERS
************************************/

void impl_expr_free(Expr* expr);

#endif //HG_COLTI_AST