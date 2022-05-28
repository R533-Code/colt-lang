/** @file ast.c
* Contains the definitions of the functions declared in 'ast.h'
*/

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
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	if (ast->expr = impl_binary_expr(ast, 0))
		return true;
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
		UnaryExpr* uexpr = (UnaryExpr*)expr;
		//we need to recurse to free expressions of expressions
		impl_expr_free(uexpr->child);
		safe_free(uexpr);
	}
	break; case EXPR_BINARY:
	{
		BinaryExpr* bexpr = (BinaryExpr*)expr;
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
		10, 13, 0,
		10, 13, 0,
		11, 0,
		11, 0,
		8, 9, 8,
		8, 9, 8,
		0, 7,
		12, 7,
		6, 0, 3,
		4, 0, 2,
		5, 0,
		12, 11
	};
	colti_assert(token >= 0, "Token should be greater or equal to 0!");
	if (token < TKN_OPERATOR_LESS_COLON)
		return operator_precedence_table[token];
	impl_scanner_print_error(&ast->scan, "Expected an operator!");
	return -1;
}

Expr* impl_binary_expr(AST* ast, int op_precedence)
{
	Expr* left;
	Token token_type;
	
	left = impl_primary_expr(ast);
	
	token_type = ast->current_tkn;
	if (token_type == TKN_EOF)
		return left;

	while (impl_op_precedence(ast, token_type) > op_precedence)
	{
		//Read the next token
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		Expr* right = impl_binary_expr(ast, impl_op_precedence(ast, token_type));

		left = make_binary_expr(left, token_type, right);

		token_type = ast->current_tkn;
		if (token_type == TKN_EOF)
			return left;
	}

	return left;
}

Expr* impl_primary_expr(AST* ast)
{
	Expr* primary;
	QWORD value;

	switch (ast->current_tkn)
	{
	break; case TKN_INTEGER:
		value.ui64 = ScannerGetInt(&ast->scan);
		primary = make_literal_expr(value, COLTI_UINT64);
	break; case TKN_DOUBLE:
		value.d = ScannerGetDouble(&ast->scan);
		primary = make_literal_expr(value, COLTI_DOUBLE);
	break; case TKN_TRUE:
		value.b = true;
		primary = make_literal_expr(value, COLTI_BOOL);
	break; case TKN_FALSE:
		value.b = false;
		primary = make_literal_expr(value, COLTI_BOOL);
	break; default:
		impl_scanner_print_error(&ast->scan, "Expected an expression!");
	}
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return primary;
}