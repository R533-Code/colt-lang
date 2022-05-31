/** @file ast.c
* Contains the definitions of the functions declared in 'ast.h'
*/

#include "ast.h"

void ASTInit(AST* ast, StringView to_parse)
{
	ast->error_nb = 0;
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
	ast->expr = impl_binary_expr(ast, 0);
	if (ast->error_nb != 0)
		return false;
	return true;
}

void ast_gen_error(AST* ast, const char* format, ...)
{
	StringView current_line = ScannerGetCurrentLine(&ast->scan);
	StringView current_lexeme = ScannerGetCurrentLexeme(&ast->scan);
	
	//We clear all the token till we hit an EOF or ';'
	//This allows to clear an expression that is wrong
	//also updates error number
	ast_enter_panic_mode(ast);
	
	fprintf(stderr, CONSOLE_FOREGROUND_BRIGHT_RED "Error: "
		CONSOLE_COLOR_RESET "On line %"PRIu64": ", ast->scan.current_line);
	//prints the error
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputc('\n', stderr);

	//To highlight the error lexeme, we need to break down the line in 3 parts:
	//The part before the error, the error, and the part after the error
	fprintf(stderr, "%.*s"CONSOLE_BACKGROUND_BRIGHT_RED"%.*s"CONSOLE_COLOR_RESET"%.*s\n",
		(uint32_t)(current_lexeme.start - current_line.start), current_line.start,
		(uint32_t)(current_lexeme.end - current_lexeme.start), current_lexeme.start,
		(uint32_t)(current_line.end - current_lexeme.end), current_lexeme.end
	);
}

void ast_enter_panic_mode(AST* ast)
{
	ast->error_nb++;
	while (ast->current_tkn != TKN_EOF && ast->current_tkn != TKN_SEMICOLON)
	{
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
	}
}

/************************************
IMPLEMENTATION HELPERS
************************************/

int impl_op_precedence(AST* ast, Token token)
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
	ast_gen_error(ast, "Expected an operator!");
	return 100;
}

Expr* impl_binary_expr(AST* ast, int op_precedence)
{
	if (op_precedence == 100) //token was not an operator: error
		return NULL;

	Expr* left;
	Token token_type;
	
	left = impl_primary_expr(ast);
	
	token_type = ast->current_tkn;

	switch (token_type)
	{
	case TKN_ERROR:
		ast_enter_panic_mode(ast);
		//fall-through done on purpose
	case TKN_EOF:
	case TKN_RIGHT_PAREN:
	case TKN_SEMICOLON:
		return left;
	}

	uint64_t line_nb = ast->scan.current_line;
	StringView lexeme_strv = ScannerGetCurrentLexeme(&ast->scan);
	StringView line_strv = ScannerGetCurrentLine(&ast->scan);

	int precedence = impl_op_precedence(ast, token_type);
	
	while (precedence > op_precedence)
	{
		if (precedence == 100) //token was not an operator: error
			return left; // we don't want memory leaks

		//Read the next token
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		Expr* right = impl_binary_expr(ast, impl_op_precedence(ast, token_type));
		if (!right)
			return left; // we don't want memory leaks


		//HANDLE CONVERSIONS
		if (right->expr_type.type_id != left->expr_type.type_id)
		{
			Type cnv = impl_builtin_inter_type(left->expr_type, right->expr_type);
			if (right->expr_type.type_id != cnv.type_id)
				right = makeConvertExpr(right, cnv, right->line_nb, right->line, right->lexeme);
			if (left->expr_type.type_id != cnv.type_id)
				left = makeConvertExpr(left, cnv, left->line_nb, left->line, left->lexeme);
		}

		left = makeBinaryExpr(left, token_type, right,
			line_nb,
			line_strv,
			lexeme_strv);

		token_type = ast->current_tkn;
		
		switch (token_type)
		{
		case TKN_ERROR:
			ast_enter_panic_mode(ast);
			//fall-through done on purpose
		case TKN_EOF:
		case TKN_RIGHT_PAREN:
		case TKN_SEMICOLON:
			return left;
		}

		precedence = impl_op_precedence(ast, token_type);
	}

	return left;
}

Expr* impl_primary_expr(AST* ast)
{
	Expr* primary;
	//Zero initialize value. This is a really important step,
	//as it allows faster conversions in OpCode_Convert
	QWORD value = { .u64 = 0 };
	Type type;

	switch (ast->current_tkn)
	{
		/**************** LITERALS ****************/

	break; case TKN_I8:
		value.i8 = ast->scan.parsed_value.i8;
		type = ColtInt8;
	break; case TKN_I16:
		value.i16 = ast->scan.parsed_value.i16;
		type = ColtInt16;
	break; case TKN_I32:
		value.i32 = ast->scan.parsed_value.i32;
		type = ColtInt32;
	break; case TKN_I64:
		value.i64 = ast->scan.parsed_value.i64;
		type = ColtInt64;
	break; case TKN_U8:
		value.u8 = ast->scan.parsed_value.u8;
		type = ColtUInt8;
	break; case TKN_U16:
		value.u16 = ast->scan.parsed_value.u16;
		type = ColtUInt16;
	break; case TKN_U32:
		value.u32 = ast->scan.parsed_value.u32;
		type = ColtUInt32;
	break; case TKN_U64:
		value.u64 = ast->scan.parsed_value.u64;
		type = ColtUInt64;
	break; case TKN_FLOAT:
		value.f = ast->scan.parsed_value.f;
		type = ColtFloat;
	break; case TKN_DOUBLE:
		value.d = ast->scan.parsed_value.d;
		type = ColtDouble;
	break; case TKN_BOOL:
		value.b = ast->scan.parsed_value.b;
		type = ColtBool;
	break;
	
		/**************** UNARY OPERATORS ****************/
	
	case TKN_OPERATOR_MINUS:
	case TKN_OPERATOR_PLUS:
	case TKN_OPERATOR_TILDE:
	case TKN_OPERATOR_BANG:
		//there is no need updating token as a unary expression's internal already does that
		primary = impl_unary_expr(ast);
		return primary;

		/**************** PARENTHESIS ****************/

	break; case TKN_LEFT_PAREN:
		primary = impl_paren_expr(ast);
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		return primary;
	
		/**************** ERROR ****************/

	break; case TKN_ERROR:
		ast_enter_panic_mode(ast);
		return NULL;

	break; default:
		ast_gen_error(ast, "Expected an expression!");
		return NULL;
	}
	primary = makeLiteralExpr(value, type,
		ast->scan.current_line,
		ScannerGetCurrentLine(&ast->scan),
		ScannerGetCurrentLexeme(&ast->scan));
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return primary;
}

Expr* impl_unary_expr(AST* ast)
{
	//As C function argument order is not guaranteed
	StringView lexeme_strv = ScannerGetCurrentLexeme(&ast->scan);
	StringView line_strv = ScannerGetCurrentLine(&ast->scan);
	Token tkn = ast->current_tkn;
	
	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	Expr* child = impl_primary_expr(ast);
	if (!child)
		return NULL; //propagate the error

	return makeUnaryExpr(tkn, child,
		ast->scan.current_line,
		line_strv,
		lexeme_strv);
}

Expr* impl_paren_expr(AST* ast)
{
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	Expr* ret = impl_binary_expr(ast, 0);
	if (ast->current_tkn != TKN_RIGHT_PAREN)
	{
		ast_gen_error(ast, "Expected a closing parenthesis ')'!");		
	}
	
	return ret;
}