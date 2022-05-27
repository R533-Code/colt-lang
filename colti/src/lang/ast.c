#include "ast.h"

void ASTInit(AST* ast, StringView to_parse)
{

}

void ASTFree(AST* ast)
{
	ScannerFree(&ast->scan);
	impl_expr_free(ast->expr);
	
	DO_IF_DEBUG_BUILD(
		ast->expr = NULL;
	);
}

/************************************
IMPLEMENTATION HELPERS
************************************/

void impl_expr_free(Expr* expr)
{
	switch (expr->identifier)
	{
	break; case EXPR_VAR:
	{
		//TODO: when implemented
	}
	break; case EXPR_FN:
	{
		//TODO: when implemented
	}
	break; case EXPR_UNARY:
	{
		UnaryExpr* uexpr = expr;
		//we need to recurse to free expressions of expressions
		impl_expr_free(uexpr->child);
		safe_free(uexpr);
	}
	break; case EXPR_BINARY:
	{
		BinaryExpr* bexpr = expr;
		//we need to recurse to free expressions of expressions
		impl_expr_free(bexpr->lhs);
		impl_expr_free(bexpr->rhs);
		safe_free(bexpr);
	}
	break; case EXPR_LITERAL:
		safe_free(expr);
	break; default:
		colti_assert(false, "Expression identifier was invalid!");
	}
}
