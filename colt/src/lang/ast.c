/** @file ast.c
* Contains the definitions of the functions declared in 'ast.h'
*/

#include "ast.h"

void ASTInit(AST* ast)
{
	ast->error_nb = 0;
	ast->warning_nb = 0;
	ExprArrayInit(&ast->expr);
	ASTTableInit(&ast->table);
}

void ASTFree(AST* ast)
{
	ASTTableFree(&ast->table);
	ExprArrayFree(&ast->expr);
}

bool ASTParse(AST* ast, StringView to_parse, const ColtScanOptions* options)
{
	colt_assert(options != NULL, "Options should not be NULL!");
	ast->error_nb = 0;
	ast->warning_nb = 0;
	ast->options = options;
	//TODO: add a ExprArrayReset function
	ExprArrayFree(&ast->expr);
	ExprArrayInit(&ast->expr);
	ScannerInit(&ast->scan, to_parse);
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	
	while (ast->current_tkn != TKN_EOF)
	{
		//parse the expression
		ExprArrayPushBack(&ast->expr, parse_expression(ast));
	}

	ScannerFree(&ast->scan);
	if (ast->error_nb != 0)
		return false;
	return true;
}

void ASTReset(AST* ast)
{
	ExprArrayFree(&ast->expr);
	ExprArrayInit(&ast->expr);

	VariableTableFree(&ast->table.var_table);
	VariableTableInit(&ast->table.var_table);
	ast->error_nb = 0;
	ast->warning_nb = 0;
}

void ast_gen_warning(AST* ast, uint64_t line_nb, StringView line, StringView lexeme, const char* format, ...)
{
	ast->warning_nb++;
	//We still update the warning number, but do not print anything
	if (ast->options->no_warning == true)
		return;

	fprintf(stdout, CONSOLE_FOREGROUND_BRIGHT_YELLOW "Warning: "
		CONSOLE_COLOR_RESET "On line %"PRIu64": ", line_nb);
	//prints the error
	va_list args;
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
	fputc('\n', stdout);

	//To highlight the error lexeme, we need to break down the line in 3 parts:
	//The part before the error, the error, and the part after the error
	fprintf(stdout, "%.*s"CONSOLE_FOREGROUND_BRIGHT_YELLOW"%.*s"CONSOLE_COLOR_RESET"%.*s\n",
		(uint32_t)(lexeme.start - line.start), line.start,
		(uint32_t)(lexeme.end - lexeme.start), lexeme.start,
		(uint32_t)(line.end - lexeme.end), lexeme.end
	);
}

void ast_gen_error(AST* ast, uint64_t line_nb, StringView current_line, StringView current_lexeme, const char* format, ...)
{
	//We clear all the token till we hit an EOF or ';' or '}' or ')'
	//This allows to clear an expression that is wrong
	//also updates error number
	ast_enter_panic_mode(ast);

	//We do not want to print anything
	if (ast->options->no_error == true)
		return;

	fprintf(stderr, CONSOLE_FOREGROUND_BRIGHT_RED "Error: "
		CONSOLE_COLOR_RESET "On line %"PRIu64": ", line_nb);
	//prints the error
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputc('\n', stderr);

	if ((uint32_t)(current_lexeme.end - current_lexeme.start) == 0)
	{
		//To highlight the error lexeme, we need to break down the line in 3 parts:
		//The part before the error, the error, and the part after the error
		fprintf(stderr, "%.*s"CONSOLE_BACKGROUND_BRIGHT_RED" "CONSOLE_COLOR_RESET"%.*s\n",
			(uint32_t)(current_lexeme.start - current_line.start), current_line.start,
			(uint32_t)(current_line.end - current_lexeme.end), current_lexeme.end
		);
	}
	else
	{
		//To highlight the error lexeme, we need to break down the line in 3 parts:
		//The part before the error, the error, and the part after the error
		fprintf(stderr, "%.*s"CONSOLE_BACKGROUND_BRIGHT_RED"%.*s"CONSOLE_COLOR_RESET"%.*s\n",
			(uint32_t)(current_lexeme.start - current_line.start), current_line.start,
			(uint32_t)(current_lexeme.end - current_lexeme.start), current_lexeme.start,
			(uint32_t)(current_line.end - current_lexeme.end), current_lexeme.end
		);
	}
}

void ast_enter_panic_mode(AST* ast)
{
	ast->error_nb++;
	while (ast->current_tkn != TKN_RIGHT_PAREN && ast->current_tkn != TKN_RIGHT_CURLY && ast->current_tkn != TKN_EOF && ast->current_tkn != TKN_SEMICOLON)
	{
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
	}
}

void ast_handle_conversion(Expr** plhs, Expr** prhs)
{
	Expr* lhs = *plhs;
	Expr* rhs = *prhs;

	if (lhs->expr_type.type_id == ID_COLT_LSTRING || rhs->expr_type.type_id == ID_COLT_LSTRING)
		return;

	if (lhs->expr_type.type_id != rhs->expr_type.type_id)
	{
		Type cnv = builtin_inter_type(lhs->expr_type, rhs->expr_type);
		if (lhs->expr_type.type_id != cnv.type_id)
			*plhs = makeConvertExpr(lhs, cnv, lhs->line_nb, lhs->line, lhs->lexeme);
		if (rhs->expr_type.type_id != cnv.type_id)
			*prhs = makeConvertExpr(rhs, cnv, rhs->line_nb, rhs->line, rhs->lexeme);
	}
	return;
}

Type ast_operator_return_type(AST* ast, Type lhs, Token binary_op, Type rhs, uint64_t line_nb, StringView line, StringView lexeme)
{
	if (lhs.type_id == ID_COLT_LSTRING || rhs.type_id == ID_COLT_LSTRING)
	{
		ast_gen_error(ast, line_nb, line, lexeme, "'%.*s' cannot have an 'lstring' as operand!", (uint32_t)(lexeme.end - lexeme.start), lexeme.start);
		return ColtBool;
	}

	switch (binary_op)
	{
	case TKN_OPERATOR_PLUS:
	case TKN_OPERATOR_MINUS:
	case TKN_OPERATOR_STAR:
	case TKN_OPERATOR_SLASH:
	case TKN_OPERATOR_EQUAL:
	case TKN_OPERATOR_PLUS_EQUAL:
	case TKN_OPERATOR_MINUS_EQUAL:
	case TKN_OPERATOR_STAR_EQUAL:
	case TKN_OPERATOR_SLASH_EQUAL:
		return lhs;
	case TKN_OPERATOR_AND:
	case TKN_OPERATOR_OR:
	case TKN_OPERATOR_XOR:
	case TKN_OPERATOR_GREATER_GREATER:
	case TKN_OPERATOR_LESS_LESS:
	case TKN_OPERATOR_AND_EQUAL:
	case TKN_OPERATOR_OR_EQUAL:
	case TKN_OPERATOR_XOR_EQUAL:
	case TKN_OPERATOR_MODULO:
		if (is_type_integral(lhs.type_id) && is_type_integral(rhs.type_id))
			return lhs;
		else
		{
			ast_gen_error(ast, line_nb, line, lexeme, "'%.*s' expects integral operands!", (uint32_t)(lexeme.end - lexeme.start), lexeme.start);
			return ColtBool;
		}
	case TKN_OPERATOR_GREATER:
	case TKN_OPERATOR_GREATER_EQUAL:
	case TKN_OPERATOR_LESS:
	case TKN_OPERATOR_LESS_EQUAL:
	case TKN_OPERATOR_EQUAL_EQUAL:
	case TKN_OPERATOR_BANG_EQUAL:
	case TKN_OPERATOR_AND_AND:
	case TKN_OPERATOR_OR_OR:
		return ColtBool;

	default:
		//error already outputted by impl_op_precedence 
		return ColtBool;
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
	colt_assert(token >= 0, "Token should be greater or equal to 0!");
	if (token < TKN_OPERATOR_LESS_COLON)
		return operator_precedence_table[token];
	return 100;
}

Expr* parse_binary(AST* ast, int op_precedence)
{
	if (op_precedence == 100) //token was not an operator: error
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected an operator!");
		return NULL;
	}

	Expr* left = parse_primary(ast);
	if (!left)
		return NULL;

	Token bin_operator = ast->current_tkn;

	switch (bin_operator)
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

	int precedence = impl_op_precedence(ast, bin_operator);

	while (precedence > op_precedence)
	{
		if (precedence == 100) //token was not an operator: error
		{
			ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
				"Expected an operator!");
			return left;
		}

		//Read the next token
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		Expr* right = parse_binary(ast, impl_op_precedence(ast, bin_operator));
		if (!right) //propagate error
			return left; // we don't want memory leaks

		//left hand side should be an lvalue
		if (is_assignment_token(bin_operator) && !(left->identifier == EXPR_VAR || is_assignment_expr(left)))
			ast_gen_error(ast, left->line_nb, left->line, left->lexeme, "Left hand side of an assignment should be a variable (lvalue)!");

		Type expr_type;

		if (is_assignment_token(bin_operator))
		{
			expr_type = left->expr_type;
			right = makeConvertExpr(right, left->expr_type,
				right->line_nb, right->line, right->lexeme);
		}
		else
		{
			ast_handle_conversion(&left, &right);
			expr_type = ast_operator_return_type(ast, left->expr_type, bin_operator, right->expr_type,
				line_nb, line_strv, lexeme_strv);
		}

		//Pratt's parsing, which allows operators priority
		left = makeBinaryExpr(left, bin_operator, right, expr_type,
			line_nb,
			line_strv,
			lexeme_strv);

		bin_operator = ast->current_tkn;

		switch (bin_operator)
		{
		case TKN_ERROR:
			ast_enter_panic_mode(ast);
			//fall-through done on purpose
		case TKN_EOF:
		case TKN_RIGHT_PAREN:
		case TKN_SEMICOLON:
			return left;
		}

		precedence = impl_op_precedence(ast, bin_operator);
	}

	return left;
}

Expr* parse_primary(AST* ast)
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
		type = ColtI8;
	break; case TKN_I16:
		value.i16 = ast->scan.parsed_value.i16;
		type = ColtI16;
	break; case TKN_I32:
		value.i32 = ast->scan.parsed_value.i32;
		type = ColtI32;
	break; case TKN_I64:
		value.i64 = ast->scan.parsed_value.i64;
		type = ColtI64;
	break; case TKN_U8:
		value.u8 = ast->scan.parsed_value.u8;
		type = ColtU8;
	break; case TKN_U16:
		value.u16 = ast->scan.parsed_value.u16;
		type = ColtU16;
	break; case TKN_U32:
		value.u32 = ast->scan.parsed_value.u32;
		type = ColtU32;
	break; case TKN_U64:
		value.u64 = ast->scan.parsed_value.u64;
		type = ColtU64;
	break; case TKN_FLOAT:
		value.f = ast->scan.parsed_value.f;
		type = ColtFloat;
	break; case TKN_DOUBLE:
		value.d = ast->scan.parsed_value.d;
		type = ColtDouble;
	break; case TKN_BOOL:
		value.b = ast->scan.parsed_value.b;
		type = ColtBool;

	break; case TKN_STRING:
	{
		value.string_ptr = ScannerGetLString(&ast->scan);
		type = ColtLString;
		//Add string to string literal table
		StringTableAdd(&ast->table.str_table, value.string_ptr);
	}
	break;

		/**************** UNARY OPERATORS ****************/

	case TKN_OPERATOR_MINUS:
	case TKN_OPERATOR_PLUS:
	case TKN_OPERATOR_TILDE:
	case TKN_OPERATOR_BANG:
		//there is no need updating current token as a unary expression's internal already does that
		primary = parse_unary(ast);
		return primary;

		/**************** PARENTHESIS ****************/

	break; case TKN_LEFT_PAREN:
		primary = parse_parenthesis(ast);
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		return primary;

		/**************** VARIABLE ****************/
	
	break; case TKN_IDENTIFIER:
	{
		StringView variable_name = ScannerGetIdentifier(&ast->scan);
		const VariableEntry* table_entry = VariableTableGetEntry(&ast->table.var_table, variable_name);
		if (!table_entry)
		{
			ast_gen_error(ast,
				ast->scan.current_line,
				ScannerGetCurrentLine(&ast->scan),
				ScannerGetCurrentLexeme(&ast->scan),
				"Identifier '%.*s' is not defined!", (uint32_t)(variable_name.end - variable_name.start), variable_name.start
			);
			return NULL;
		}
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		return makeVariableExpr(variable_name, table_entry->type,
			ast->scan.current_line,
			ScannerGetCurrentLine(&ast->scan),
			ScannerGetCurrentLexeme(&ast->scan));
	}

		/**************** ERROR ****************/

	break; case TKN_ERROR:
		ast_enter_panic_mode(ast);
		return NULL;

	break; default:
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected an expression, or a unary operator [+-~!]!");
		return NULL;
	}
	primary = makeLiteralExpr(value, type,
		ast->scan.current_line,
		ScannerGetCurrentLine(&ast->scan),
		ScannerGetCurrentLexeme(&ast->scan));
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return primary;
}

Expr* parse_unary(AST* ast)
{
	//As C function argument order is not guaranteed
	StringView lexeme_strv = ScannerGetCurrentLexeme(&ast->scan);
	StringView line_strv = ScannerGetCurrentLine(&ast->scan);
	//the unary operator
	Token unary_op = ast->current_tkn;

	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	Expr* child = parse_primary(ast);
	if (!child)
		return NULL; //propagate the error

	//a minus followed by a unsigned type is converted to a signed type
	if (is_type_unsigned_int(child->expr_type.type_id) && unary_op == TKN_OPERATOR_MINUS)
	{
		//TODO: add option check
		ast_gen_warning(ast, child->line_nb, child->line, child->lexeme, "Implicit conversion from '%s' to '%s'!", BuiltinTypeIDToString(child->expr_type.type_id), BuiltinTypeIDToString((BuiltinTypeID)child->expr_type.type_id + 4));
		child = makeConvertExpr(child, type_unsigned_to_signed(child->expr_type.type_id),
			child->line_nb, child->line, child->lexeme
		);
	}

	Type expr_type;
	switch (unary_op)
	{
	break; case TKN_OPERATOR_BANG:
		expr_type = ColtBool;
	break; default:
		expr_type = child->expr_type;
	}

	return makeUnaryExpr(unary_op, child, expr_type,
		ast->scan.current_line,
		line_strv,
		lexeme_strv);
}

Expr* parse_parenthesis(AST* ast)
{
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	Expr* ret = parse_binary(ast, 0);
	if (ast->current_tkn != TKN_RIGHT_PAREN)
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected a closing parenthesis ')'!");
	}

	return ret;
}

Expr* parse_expression(AST* ast)
{
	Expr* expr;
	switch (ast->current_tkn)
	{
	case TKN_KEYWORD_VAR:
	case TKN_BUILTIN_TYPE:
		expr = parse_variable_declaration(ast);
	break; default:
		expr = parse_binary(ast, -1);
		if (ast->options->no_warn_unused_result == false && ast->error_nb == 0)
			ast_gen_warning(ast, expr->line_nb, expr->line, expr->lexeme, "Unused expression result!");
	}
	if (ast->current_tkn != TKN_SEMICOLON && ast->current_tkn != TKN_ERROR && ast->current_tkn != TKN_EOF)
	{
		ast_gen_error(ast,
			ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected a semicolon ';'!"
		);
	}
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return expr;
}

Expr* parse_variable_declaration(AST* ast)
{
	Token tkn_type = ast->current_tkn;
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	if (ast->current_tkn != TKN_IDENTIFIER)
	{
		ast_gen_error(ast,
			ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected an identifier!"
		);
		return NULL;
	}
	Type var_type;
	if (tkn_type != TKN_KEYWORD_VAR)
		var_type = ScannerGetTypename(&ast->scan);
	StringView decl_identifier = ScannerGetIdentifier(&ast->scan);
	StringView identifier_line = ScannerGetCurrentLine(&ast->scan);
	uint64_t identifier_line_nb = ast->scan.current_line;

	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	if (ast->current_tkn == TKN_SEMICOLON)
	{
		if (tkn_type == TKN_KEYWORD_VAR)
		{
			ast_gen_error(ast,
				ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
				"Variable declared with 'var' should always be initialized!"
			);
			return NULL;
		}
		if (ast->options->no_warn_uninitialized == false)
			ast_gen_warning(ast, identifier_line_nb, identifier_line, decl_identifier, "\"%.*s\" is not initialized!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);

		if (VariableTableContains(&ast->table.var_table, decl_identifier))
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start
			);
			return NULL;
		}

		QWORD zero = { .u64 = 0 };
		var_type = ScannerGetTypename(&ast->scan);
		VariableTableSet(&ast->table.var_table, decl_identifier, zero, var_type);

		return makeVariableExpr(decl_identifier, var_type,
			identifier_line_nb, identifier_line, decl_identifier);
	}
	else if (ast->current_tkn == TKN_OPERATOR_EQUAL)
	{
		//The operator source code's location
		StringView line = ScannerGetCurrentLine(&ast->scan);
		StringView lexeme = ScannerGetCurrentLexeme(&ast->scan);
		uint64_t line_nb = ast->scan.current_line;

		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		Expr* to_assign = parse_binary(ast, 0);
		if (!to_assign)
			return NULL;
		
		//we don't want to register the variable if the expression is not valid
		if (ast->current_tkn != TKN_SEMICOLON)
			return to_assign;
		
		if (tkn_type == TKN_KEYWORD_VAR)
			var_type = to_assign->expr_type;
		else if (var_type.type_id != to_assign->expr_type.type_id)
		{
			//TODO: issue conversion warnings
			to_assign = makeConvertExpr(to_assign, var_type,
				to_assign->line_nb, to_assign->line, to_assign->lexeme
				);
		}


		if (VariableTableContains(&ast->table.var_table, decl_identifier))
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start
			);
			return to_assign;
		}
		
		QWORD zero = { .u64 = 0 };
		VariableTableSet(&ast->table.var_table, decl_identifier, zero, to_assign->expr_type);

		return makeBinaryExpr(
			makeVariableExpr(decl_identifier, to_assign->expr_type,
				identifier_line_nb, identifier_line, decl_identifier),
			TKN_OPERATOR_EQUAL, to_assign, var_type, line_nb, lexeme, line
		);
	}
	return NULL;
}

bool is_assignment_token(Token tkn)
{
	switch (tkn)
	{
	case TKN_OPERATOR_EQUAL:
	case TKN_OPERATOR_AND_EQUAL:
	case TKN_OPERATOR_OR_EQUAL:
	case TKN_OPERATOR_XOR_EQUAL:
	case TKN_OPERATOR_PLUS_EQUAL:
	case TKN_OPERATOR_MINUS_EQUAL:
	case TKN_OPERATOR_STAR_EQUAL:
	case TKN_OPERATOR_SLASH_EQUAL:
		return true;
	default:
		return false;
	}
}

bool is_assignment_expr(Expr* expr)
{
	if (expr->identifier == EXPR_BINARY)
	{
		if (is_assignment_token(((BinaryExpr*)expr)->expr_operator))
			return true;
		return false;
	}
	return false;
}
