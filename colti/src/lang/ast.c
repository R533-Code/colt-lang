#include "ast.h"

void ASTInit(AST* ast, StringView to_parse)
{
	ScannerInit(&ast->scan, to_parse);
}

void ASTFree(AST* ast)
{
	ScannerFree(&ast->scan);
	impl_expr_free(ast->expr);
	
	DO_IF_DEBUG_BUILD(
		ast->expr = NULL;
	);
}

bool ASTParse(AST* ast)
{

	return false;
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

int impl_op_precedence(const AST* ast, Token token)
{
	static const int operator_precedence_table[] = 
	{
		4, 1, 14,
		4, 1, 14,
		3, 14,
		3, 14,
		6, 5, 6,
		6, 5, 6,
		14, 7,
		2, 7,
		8, 14, 11,
		10, 14, 12,
		9, 14,
		2, 3
	};
	
	if (token < TKN_OPERATOR_LESS_COLON)
		return operator_precedence_table[token];
	impl_scanner_print_error(&ast->scan, "Expected an operator!");
	return -1;
}
