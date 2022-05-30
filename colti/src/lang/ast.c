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
	freeExpr(ast->expr);
	
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

		left = makeBinaryExpr(left, token_type, right);

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
		primary = makeLiteralExpr(value, ColtUInt64);
	break; case TKN_DOUBLE:
		value.d = ScannerGetDouble(&ast->scan);
		primary = makeLiteralExpr(value, ColtDouble);
	break; case TKN_TRUE:
		value.b = true;
		primary = makeLiteralExpr(value, ColtBool);
	break; case TKN_FALSE:
		value.b = false;
		primary = makeLiteralExpr(value, ColtBool);
	break; default:
		impl_scanner_print_error(&ast->scan, "Expected an expression!");
		//FIXME: error handling
		exit(1);
	}
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return primary;
}