/** @file ast.c
* Contains the definitions of the functions declared in 'ast.h'
*/

#include "ast.h"

void ASTInit(AST* ast)
{
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
	
	//Reset state and options
	ast->error_nb = 0;
	ast->warning_nb = 0;
	ast->options = options;
	ast->is_parsing_loop = false;
	ast->current_scope = NULL;

	//Reset the array
	ExprArrayClear(&ast->expr);

	ScannerInit(&ast->scan, to_parse);
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	
	while (ast->current_tkn != TKN_EOF)
	{
		Expr* ptr = parse_expression(ast);
		if (ptr)
			ExprArrayPushBack(&ast->expr, ptr);
	}

	ScannerFree(&ast->scan);
	//If no expression could be parsed, or there is an error
	if (ast->error_nb != 0 || ast->expr.count == 0)
		return false;
	return true;
}

void ASTReset(AST* ast)
{
	ExprArrayClear(&ast->expr);
	ASTTableClear(&ast->table);
	ast->error_nb = 0;
	ast->warning_nb = 0;
}

/************************************
IMPLEMENTATION HELPERS
************************************/

Expr* parse_expression(AST* ast)
{
	Expr* expr;

	switch (ast->current_tkn)
	{
	break; case TKN_LEFT_CURLY:
		expr = parse_scope(ast);
	break; case TKN_KEYWORD_IF:
		expr = parse_conditional(ast);
	break; case TKN_KEYWORD_WHILE:
		expr = parse_while(ast);
	break; default:
	{
		switch (ast->current_tkn)
		{
		case TKN_KEYWORD_VAR:
		case TKN_BUILTIN_TYPE:
			expr = parse_variable_declaration(ast, false);
		break; case TKN_KEYWORD_BREAK:
			if (!ast->is_parsing_loop)
			{
				ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan), "Unexpected 'break': this statement can only appear in a loop!");
				return NULL;
			}
			expr = makeBreakExpr(ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan));
			ast->current_tkn = ScannerGetNextToken(&ast->scan);
		break; case TKN_KEYWORD_CONTINUE:
			if (!ast->is_parsing_loop)
			{
				ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan), "Unexpected 'break': this statement can only appear in a loop!");
				return NULL;
			}
			expr = makeContinueExpr(ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan));
			ast->current_tkn = ScannerGetNextToken(&ast->scan);
		break; case TKN_KEYWORD_CONST:
			//Consume 'const'
			ast->current_tkn = ScannerGetNextToken(&ast->scan);
			expr = parse_variable_declaration(ast, true);

		break; case TKN_SEMICOLON:
			//EMPTY EXPRESSION, the AST will not save it
			expr = NULL;

		break; default:
			expr = parse_binary(ast, 0);
			if (ast->options->no_warn_unused_result == false && !(is_assignment_expr(expr) || ExprTypeEqualTypeID(expr, ID_COLT_VOID)))
			{
				ast_gen_warning(ast, expr->line_nb, expr->line, expr->lexeme, "Unused expression result!");
				freeExpr(expr);
				return NULL;
			}
		}
		if (ast->current_tkn != TKN_SEMICOLON && ast->current_tkn != TKN_ERROR && ast->current_tkn != TKN_EOF)
		{
			ast_gen_error(ast,
				ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
				"Expected a semicolon ';'!"
			);
		}
		//so '}' missing is not generated
		if (ast->current_tkn != TKN_RIGHT_CURLY)
			ast->current_tkn = ScannerGetNextToken(&ast->scan);
	}
	}

	return expr;
}

Expr* parse_binary(AST* ast, uint8_t op_precedence)
{
	if (op_precedence == UINT8_MAX) //token was not an operator: error
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected an operator!");
		return NULL;
	}

	Expr* left = parse_primary(ast);
	if (!left)
		return NULL;

	//Assignments are handled differently:
	// 10 + 10 + 10 is read as ((10 + 10) + 10), (left associative) while for an assignment
	// a1 = a2 = a3 = 10 is read as (a1 = (a2 = (a3 = 10))), (right associative)
	if (is_assignment_token(ast->current_tkn))
		return parse_assignment(ast, left, ast->current_tkn);

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
	default: break;
	}

	uint64_t line_nb = ast->scan.current_line;
	StringView lexeme_strv = ScannerGetCurrentLexeme(&ast->scan);
	StringView line_strv = ScannerGetCurrentLine(&ast->scan);

	uint8_t precedence = ast_op_precedence(bin_operator);
	while (precedence > op_precedence)
	{
		if (precedence == UINT8_MAX) //token was not an operator: error
		{
			ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
				"Expected an operator!");
			return left;
		}

		//Read the next token
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		Expr* right = parse_binary(ast, ast_op_precedence(bin_operator));
		if (!right) //propagate error
			return left; // we don't want memory leaks

		if (bin_operator == TKN_OPERATOR_OR_OR || bin_operator == TKN_OPERATOR_AND_AND)
		{
			Type bool_t = { .typeinfo = &ColtBool, .is_const = false };
			left = ast_convert_to(ast, left, bool_t);
			right = ast_convert_to(ast, right, bool_t);
		}
		else
		{
			//Convert both expressions to the same type
			ast_convert_to_highest_type(ast, &left, &right);
		}			
		//Get the return type of the operator associated to the binary operator token
		//because operators like ==, >=, ... returns bool
		Type expr_type = ast_operator_return_type(ast, left->expr_type, bin_operator, right->expr_type,
			line_nb, line_strv, lexeme_strv);

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
		default:
			break;
		}

		precedence = ast_op_precedence(bin_operator);
	}

	return left;
}

Expr* parse_assignment(AST* ast, Expr* lhs, Token assignment_tkn)
{
	colt_assert(is_assignment_token(assignment_tkn), "assignment_tkn should be an assignment token!");

	if (lhs->identifier != EXPR_GLOB_READ && lhs->identifier != EXPR_LOCAL_READ)
	{
		ast_gen_error(ast, lhs->line_nb, lhs->line, lhs->lexeme, "Expected a variable (lvalue)!");
		return lhs;
	}

	StringView op_line = ScannerGetCurrentLine(&ast->scan);
	StringView op_lexeme = ScannerGetCurrentLexeme(&ast->scan);
	uint64_t op_line_nb = ast->scan.current_line;

	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	Expr* rhs = parse_binary(ast, 0);
	if (rhs == NULL)
		return lhs;
	
	//Convert 'rhs' to the same type as 'lhs'
	rhs = ast_convert_to(ast, rhs, lhs->expr_type);
	//Generates error if necessary
	Type ret_type = ast_operator_return_type(ast, lhs->expr_type, assignment_tkn, rhs->expr_type, op_line_nb, op_line, op_lexeme);

	switch (assignment_tkn)
	{
	break; case TKN_OPERATOR_AND_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_AND, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_XOR_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_XOR, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_OR_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_OR, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_MINUS_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_MINUS, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_PLUS_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_PLUS, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_STAR_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_STAR, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_SLASH_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_SLASH, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_LESS_LESS_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_LESS_LESS, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_GREATER_GREATER_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_GREATER_GREATER, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; case TKN_OPERATOR_MODULO_EQUAL:
		rhs = makeBinaryExpr(lhs, TKN_OPERATOR_MODULO, rhs, ret_type, op_line_nb, op_line, op_lexeme);
	break; default:
		break;
	}

	if (lhs->expr_type.is_const)
		ast_gen_error(ast, lhs->line_nb, lhs->line, lhs->lexeme, "Cannot assign to 'const' variables!");

	Expr* ret;
	if (lhs->identifier == EXPR_GLOB_READ)
	{
		ret = makeGlobalWriteExpr(
			((GlobalReadExpr*)lhs)->var_name, lhs->expr_type, rhs, lhs->line_nb, lhs->line, lhs->lexeme
		);
	}
	else //EXPR_LOCAL_READ
	{
		ret = makeLocalWriteExpr(
			((LocalReadExpr*)lhs)->var_name, lhs->expr_type, ((LocalReadExpr*)lhs)->offset, rhs, lhs->line_nb, lhs->line, lhs->lexeme
		);
	}

	//As we took the data from lhs, we no longer need it.
	//For the other case, the expression becomes part of the BinaryExpr
	if (assignment_tkn == TKN_OPERATOR_EQUAL)
		freeExpr(lhs);

	return ret;
}

Expr* parse_primary(AST* ast)
{
	Expr* primary;
	//Zero initialize value. This is a really important step,
	//as it allows faster conversions in OpCode_Convert
	QWORD value = { .u64 = 0 };
	Type type = { .is_const = false };

	switch (ast->current_tkn)
	{
		/**************** LITERALS ****************/

	break; case TKN_I8:
		value.i8 = ast->scan.parsed_value.i8;
		type.typeinfo = &ColtI8;
	break; case TKN_I16:
		value.i16 = ast->scan.parsed_value.i16;
		type.typeinfo = &ColtI16;
	break; case TKN_I32:
		value.i32 = ast->scan.parsed_value.i32;
		type.typeinfo = &ColtI32;
	break; case TKN_I64:
		value.i64 = ast->scan.parsed_value.i64;
		type.typeinfo = &ColtI64;
	break; case TKN_U8:
		value.u8 = ast->scan.parsed_value.u8;
		type.typeinfo = &ColtU8;
	break; case TKN_U16:
		value.u16 = ast->scan.parsed_value.u16;
		type.typeinfo = &ColtU16;
	break; case TKN_U32:
		value.u32 = ast->scan.parsed_value.u32;
		type.typeinfo = &ColtU32;
	break; case TKN_U64:
		value.u64 = ast->scan.parsed_value.u64;
		type.typeinfo = &ColtU64;
	break; case TKN_FLOAT:
		value.f = ast->scan.parsed_value.f;
		type.typeinfo = &ColtFloat;
	break; case TKN_DOUBLE:
		value.d = ast->scan.parsed_value.d;
		type.typeinfo = &ColtDouble;
	break; case TKN_BOOL:
		value.b = ast->scan.parsed_value.b;
		type.typeinfo = &ColtBool;

	break; case TKN_STRING:
	{
		value.string_ptr = ScannerGetLString(&ast->scan);
		type.typeinfo = &ColtLString;
		//Add string to string literal table
		StringTableAdd(&ast->table.str_table, value.string_ptr);
	}
	break; case TKN_CHAR:
		value.c = ast->scan.parsed_value.c;
		type.typeinfo = &ColtChar;
	break;

		/**************** UNARY OPERATORS ****************/

	case TKN_OPERATOR_MINUS:
	case TKN_OPERATOR_PLUS:
	case TKN_OPERATOR_TILDE:
	case TKN_OPERATOR_BANG:
	case TKN_KEYWORD_STATIC_PRINT:
		//there is no need updating current token as a unary expression's internal already does that
		primary = parse_unary(ast);
		return primary;

		/**************** PARENTHESIS ****************/

	break; case TKN_LEFT_PAREN:
		return parse_paren_binary(ast);		

		/**************** VARIABLE ****************/
	
	break; case TKN_IDENTIFIER:
	{
		StringView variable_name = ScannerGetIdentifier(&ast->scan);
		
		Expr* var_ptr = ScopeExprFindVar(ast->current_scope, variable_name);
		if (var_ptr == NULL)
			return make_global_read_expr(ast, variable_name);
		
		var_ptr = makeLocalReadExpr(variable_name, var_ptr->expr_type, ((LocalReadExpr*)var_ptr)->offset,
			ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan)
		);
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		return var_ptr;
	}

		/**************** ERROR ****************/

	break; case TKN_ERROR:
		ast_enter_panic_mode(ast);
		return NULL;

	break; default:
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected an expression!");
		return NULL;
	}
	primary = makeLiteralExpr(value, type,
		ast->scan.current_line,
		ScannerGetCurrentLine(&ast->scan),
		ScannerGetCurrentLexeme(&ast->scan));
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return primary;
}

Expr* parse_paren_boolean(AST* ast)
{
	if (ast->current_tkn != TKN_LEFT_PAREN)
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan), "Expected a left parenthesis '('!");
		return NULL;
	}
	//Consume '('
	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	Type bool_t = { .typeinfo = &ColtBool, .is_const = false };
	Expr* bin = parse_binary(ast, 0);
	if (ast->current_tkn != TKN_RIGHT_PAREN)
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan), "Expected a right parenthesis ')'!");
		return bin;
	}
	//Consume ')'
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	return ast_convert_to(ast, bin, bool_t);
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
	if (is_type_unsigned_int(child->expr_type) && unary_op == TKN_OPERATOR_MINUS)
	{
		//TODO: add option check
		ast_gen_warning(ast, child->line_nb, child->line, child->lexeme, 
			"Implicit conversion from '%s' to '%s'!", 
			BuiltinTypeIDToString((BuiltinTypeID)TypeGetID(child->expr_type)), BuiltinTypeIDToString((BuiltinTypeID)TypeGetID(child->expr_type) + 4)
		);
		child = makeConvertExpr(child, type_unsigned_to_signed(child->expr_type),
			child->line_nb, child->line, child->lexeme
		);
	}

	Type expr_type = { .is_const = false };
	switch (unary_op)
	{
	break; case TKN_KEYWORD_STATIC_PRINT:
		expr_type.typeinfo = &ColtVoid;
	break; case TKN_OPERATOR_BANG:
		expr_type.typeinfo = &ColtBool;
	break; default:
		expr_type = child->expr_type;
	}
	
	return makeUnaryExpr(unary_op, child, expr_type,
		ast->scan.current_line,
		line_strv,
		lexeme_strv);
}

Expr* parse_paren_binary(AST* ast)
{
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	Expr* ret = parse_binary(ast, 0);
	if (ast->current_tkn != TKN_RIGHT_PAREN)
	{
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected a closing parenthesis ')'!");
	}
	//So that (...; only generates an error once
	if (ast->current_tkn != TKN_SEMICOLON)
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

	return ret;
}

Expr* parse_scope(AST* ast)
{
	colt_assert(ast->current_tkn == TKN_LEFT_CURLY, "Expected {!");
	
	//Save the current scope of the AST
	ScopeExpr* old_scope = ast->current_scope;

	ScopeExpr* scope = (ScopeExpr*)makeScopeExpr(old_scope);
	//Update current scope to the scope being parsed
	ast->current_scope = scope;
	
	//Consume {
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	while (ast->current_tkn != TKN_RIGHT_CURLY && ast->current_tkn != TKN_EOF)
	{
		Expr* expr = parse_expression(ast);
		if (expr != NULL)
			ExprArrayPushBack(&scope->array, expr);
	}
	if (ast->current_tkn != TKN_RIGHT_CURLY)
		ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
			"Expected a closing bracket '}'!");
	//Consume '}'
	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	if (scope->array.count == 0)
	{
		freeExpr((Expr*)scope);
		return NULL;
	}

	ast->current_scope = old_scope;
	return (Expr*)scope;
}

Expr* parse_conditional(AST* ast)
{
	ConditionExpr* cond = (ConditionExpr*)makeConditionExpr();

	//Consume 'if'
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	//Parse the condition
	cond->if_condition = parse_paren_boolean(ast);
	cond->if_execute = parse_expression(ast);

	while (ast->current_tkn == TKN_KEYWORD_ELIF)
	{
		//Consume 'elif'
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		ExprArrayPushBack(&cond->elif_conditions, parse_paren_boolean(ast));
		ExprArrayPushBack(&cond->elif_executes, parse_expression(ast));
	}

	if (ast->current_tkn == TKN_KEYWORD_ELSE)
	{
		//Consume 'else'
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
		cond->else_execute = parse_expression(ast);
	}

	return (Expr*)cond;
}

Expr* parse_while(AST* ast)
{
	colt_assert(ast->current_tkn == TKN_KEYWORD_WHILE, "Expected a while keyword!");
	
	//Save old state of AST parsing_loop
	bool old_parse_loop = ast->is_parsing_loop;
	ast->is_parsing_loop = true;
	
	//Consume WHILE
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	
	Expr* cond = parse_paren_boolean(ast);
	Expr* body = parse_expression(ast);
	if (body)
		return makeWhileExpr(cond, body);
	freeExpr(cond);

	ast->is_parsing_loop = old_parse_loop;
	return NULL;
}

Expr* parse_variable_declaration(AST* ast, bool is_const)
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
	Type var_type = { .is_const = is_const };
	
	if (tkn_type != TKN_KEYWORD_VAR)
		var_type.typeinfo = ScannerGetTypeInfo(&ast->scan);
	
	StringView decl_identifier = ScannerGetIdentifier(&ast->scan);
	StringView identifier_line = ScannerGetCurrentLine(&ast->scan);
	uint64_t identifier_line_nb = ast->scan.current_line;

	ast->current_tkn = ScannerGetNextToken(&ast->scan);

	if (ast->current_tkn == TKN_SEMICOLON)
	{
		if (is_const == true)
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable declared as 'const'%s should always be initialized!", (tkn_type == TKN_KEYWORD_VAR ? " and 'var'" : "")
			);
			return NULL;
		}
		if (tkn_type == TKN_KEYWORD_VAR)
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable declared with 'var' should always be initialized!"
			);
			return NULL;
		}
		if (ast->options->no_warn_uninitialized == false)
			ast_gen_warning(ast, identifier_line_nb, identifier_line, decl_identifier, "\"%.*s\" is not initialized!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);

		if (ast->current_scope == NULL)
		{
			if (VariableTableContains(&ast->table.glob_table, decl_identifier))
			{
				ast_gen_error(ast,
					identifier_line_nb, identifier_line, decl_identifier,
					"Global variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);
				return NULL;
			}

			QWORD zero = { .u64 = 0 };
			var_type.typeinfo = ScannerGetTypeInfo(&ast->scan);
			VariableTableSet(&ast->table.glob_table, decl_identifier, zero, var_type);

			return makeGlobalWriteExpr(decl_identifier, var_type,
				makeLiteralExpr(zero, var_type, identifier_line_nb, identifier_line, decl_identifier),
				identifier_line_nb, identifier_line, decl_identifier);
		}
		//TODO: extract in functions
		if (ScopeExprIsVarDeclared(ast->current_scope, decl_identifier))
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);
			return NULL;
		}

		QWORD zero = { .u64 = 0 };
		var_type.typeinfo = ScannerGetTypeInfo(&ast->scan);
		uint64_t var_offset = ast->current_scope->var_count++ + ScopeExprGetOffset(ast->current_scope);

		return makeLocalWriteExpr(decl_identifier, var_type, var_offset,
			makeLiteralExpr(zero, var_type, identifier_line_nb, identifier_line, decl_identifier),
			identifier_line_nb, identifier_line, decl_identifier);
	}
	else if (ast->current_tkn == TKN_OPERATOR_EQUAL)
	{
		ast->current_tkn = ScannerGetNextToken(&ast->scan);

		//save the old error
		uint16_t old_error = ast->error_nb;
		Expr* to_assign = parse_binary(ast, 0);

		if (!to_assign)
			return NULL;
		
		//we don't want to register the variable if the expression is not valid
		if (ast->current_tkn != TKN_SEMICOLON || old_error != ast->error_nb)
			return to_assign;
		
		if (tkn_type == TKN_KEYWORD_VAR)
			var_type.typeinfo = to_assign->expr_type.typeinfo;
		else if (ExprTypeEqualTypeID(to_assign, TypeGetID(var_type)))
		{
			//TODO: issue conversion warnings
			to_assign = makeConvertExpr(to_assign, var_type,
				to_assign->line_nb, to_assign->line, to_assign->lexeme
				);
		}


		if (ast->current_scope == NULL)
		{
			if (VariableTableContains(&ast->table.glob_table, decl_identifier))
			{
				ast_gen_error(ast,
					identifier_line_nb, identifier_line, decl_identifier,
					"Global variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);
				return NULL;
			}

			QWORD zero = { .u64 = 0 };			
			VariableTableSet(&ast->table.glob_table, decl_identifier, zero, var_type);

			return makeGlobalWriteExpr(decl_identifier, var_type, to_assign,
				identifier_line_nb, identifier_line, decl_identifier
			);
		}
		
		if (ScopeExprIsVarDeclared(ast->current_scope, decl_identifier))
		{
			ast_gen_error(ast,
				identifier_line_nb, identifier_line, decl_identifier,
				"Variable with identifier '%.*s' already exists!", (uint32_t)(decl_identifier.end - decl_identifier.start), decl_identifier.start);
			return NULL;
		}

		uint64_t var_offset = ast->current_scope->var_count++ + ScopeExprGetOffset(ast->current_scope);
		
		return makeLocalWriteExpr(decl_identifier, var_type, var_offset,
			to_assign, identifier_line_nb, identifier_line, decl_identifier
		);
	}
	ast_gen_error(ast, ast->scan.current_line, ScannerGetCurrentLine(&ast->scan), ScannerGetCurrentLexeme(&ast->scan),
		"Expected an '=' or ';'!");
	return NULL;
}

Expr* make_global_read_expr(AST* ast, StringView variable_name)
{
	const GlobalEntry* table_entry = VariableTableGetEntry(&ast->table.glob_table, variable_name);
	if (table_entry == NULL)
	{
		ast_gen_error(ast,
			ast->scan.current_line,
			ScannerGetCurrentLine(&ast->scan),
			ScannerGetCurrentLexeme(&ast->scan),
			"Identifier '%.*s' is not defined!", (uint32_t)(variable_name.end - variable_name.start), variable_name.start
		);
		return NULL;
	}
	Expr* ret = makeGlobalReadExpr(variable_name, table_entry->type,
		ast->scan.current_line,
		ScannerGetCurrentLine(&ast->scan),
		ScannerGetCurrentLexeme(&ast->scan));
	
	ast->current_tkn = ScannerGetNextToken(&ast->scan);
	
	return ret;
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
	case TKN_OPERATOR_LESS_LESS_EQUAL:
	case TKN_OPERATOR_GREATER_GREATER_EQUAL:
	case TKN_OPERATOR_MODULO_EQUAL:
		return true;
	default:
		return false;
	}
}

bool is_assignment_expr(const Expr* expr)
{
	if (expr->identifier == EXPR_GLOB_WRITE || expr->identifier == EXPR_GLOB_READ)
		return true;
	return false;
}

Expr* ast_convert_to(AST* ast, Expr* ptr, Type to)
{
	TypeConversion conv = ptr->expr_type.typeinfo->valid_conversions[TypeGetID(to)];
	if (conv == CONV_INVALID)
	{
		ast_gen_error(ast, ptr->line_nb, ptr->line, ptr->lexeme,
			"Incompatible types, invalid conversion from '%.*s' to '%.*s'!",
			(uint32_t)(ptr->expr_type.typeinfo->name.end - ptr->expr_type.typeinfo->name.start), ptr->expr_type.typeinfo->name.start,
			(uint32_t)(to.typeinfo->name.end - to.typeinfo->name.start), to.typeinfo->name.start);
		return ptr;
	}

	if (conv & CONV_WLOSSY)
	{
		ast_gen_warning(ast, ptr->line_nb, ptr->line, ptr->lexeme,
			"Lossy conversion from '%.*s' to '%.*s'!",
			(uint32_t)(ptr->expr_type.typeinfo->name.end - ptr->expr_type.typeinfo->name.start), ptr->expr_type.typeinfo->name.start,
			(uint32_t)(to.typeinfo->name.end - to.typeinfo->name.start), to.typeinfo->name.start);
	}
	if (conv & CONV_WSIGN)
	{
		ast_gen_warning(ast, ptr->line_nb, ptr->line, ptr->lexeme,
			"Sign mismatch in conversion from '%.*s' to '%.*s'!",
			(uint32_t)(ptr->expr_type.typeinfo->name.end - ptr->expr_type.typeinfo->name.start), ptr->expr_type.typeinfo->name.start,
			(uint32_t)(to.typeinfo->name.end - to.typeinfo->name.start), to.typeinfo->name.start);
	}

	return makeConvertExpr(ptr, to, ptr->line_nb, ptr->line, ptr->lexeme);
}

void ast_convert_to_highest_type(AST* ast, Expr** plhs, Expr** prhs)
{
	if (ExprTypeEqualExprType(*plhs, *prhs))
		return;

	//TODO: optimize this boolean away
	bool swapped = false;
	if (is_type_greater((*plhs)->expr_type, (*prhs)->expr_type))
	{
		//Swap pointers
		Expr* temp = *plhs;
		*plhs = *prhs;
		*prhs = temp;
		swapped = true;
	}

	Expr* lhs = *plhs;
	Expr* rhs = *prhs;

	TypeConversion conv = lhs->expr_type.typeinfo->valid_conversions[ExprGetID(rhs)];
	if (conv == CONV_INVALID)
	{
		ast_gen_error(ast, lhs->line_nb, lhs->line, lhs->lexeme,
			"Incompatible types, invalid conversion from '%.*s' to '%.*s'!",
			(uint32_t)(lhs->expr_type.typeinfo->name.end - lhs->expr_type.typeinfo->name.start), lhs->expr_type.typeinfo->name.start,
			(uint32_t)(rhs->expr_type.typeinfo->name.end - rhs->expr_type.typeinfo->name.start), rhs->expr_type.typeinfo->name.start);
		return;
	}

	if (conv & CONV_WLOSSY)
	{
		ast_gen_warning(ast, lhs->line_nb, lhs->line, lhs->lexeme,
			"Truncation from '%.*s' to '%.*s'!",
			(uint32_t)(lhs->expr_type.typeinfo->name.end - lhs->expr_type.typeinfo->name.start), lhs->expr_type.typeinfo->name.start,
			(uint32_t)(rhs->expr_type.typeinfo->name.end - rhs->expr_type.typeinfo->name.start), rhs->expr_type.typeinfo->name.start);
	}
	if (conv & CONV_WSIGN)
	{
		ast_gen_warning(ast, lhs->line_nb, lhs->line, lhs->lexeme,
			"Sign mismatch conversion from '%.*s' to '%.*s'!",
			(uint32_t)(lhs->expr_type.typeinfo->name.end - lhs->expr_type.typeinfo->name.start), lhs->expr_type.typeinfo->name.start,
			(uint32_t)(rhs->expr_type.typeinfo->name.end - rhs->expr_type.typeinfo->name.start), rhs->expr_type.typeinfo->name.start);
	}

	Type conv_expr = builtin_inter_type(lhs->expr_type, rhs->expr_type);
	*plhs = makeConvertExpr(lhs, conv_expr, lhs->line_nb, lhs->line, lhs->lexeme);

	if (swapped)
	{
		//Swap pointers
		Expr* temp = *plhs;
		*plhs = *prhs;
		*prhs = temp;
	}
}

Type ast_operator_return_type(AST* ast, Type lhs, Token binary_op, Type rhs, uint64_t line_nb, StringView line, StringView lexeme)
{
	switch (binary_op)
	{
	case TKN_OPERATOR_PLUS:
	case TKN_OPERATOR_MINUS:
	case TKN_OPERATOR_STAR:
	case TKN_OPERATOR_SLASH:
	case TKN_OPERATOR_PLUS_EQUAL:
	case TKN_OPERATOR_MINUS_EQUAL:
	case TKN_OPERATOR_STAR_EQUAL:
	case TKN_OPERATOR_SLASH_EQUAL:
		if (TypeGetID(lhs) == ID_COLT_LSTRING || TypeGetID(rhs) == ID_COLT_LSTRING)
		{
			ast_gen_error(ast, line_nb, line, lexeme, "'%.*s' cannot have an 'lstring' as operand!", (uint32_t)(lexeme.end - lexeme.start), lexeme.start);
			Type ret = { .is_const = false, .typeinfo = &ColtVoid };
			return ret;
		}
		//Fall through done on purpose
	case TKN_OPERATOR_EQUAL:
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
	case TKN_OPERATOR_LESS_LESS_EQUAL:
	case TKN_OPERATOR_GREATER_GREATER_EQUAL:
	case TKN_OPERATOR_MODULO_EQUAL:
		if (is_type_integral(lhs) && is_type_integral(rhs))
			return lhs;
		else
		{
			ast_gen_error(ast, line_nb, line, lexeme, "'%.*s' expects integral operands!", (uint32_t)(lexeme.end - lexeme.start), lexeme.start);
			Type ret = { .is_const = false, .typeinfo = &ColtVoid };
			return ret;
		}
	case TKN_OPERATOR_GREATER:
	case TKN_OPERATOR_GREATER_EQUAL:
	case TKN_OPERATOR_LESS:
	case TKN_OPERATOR_LESS_EQUAL:
	case TKN_OPERATOR_AND_AND:
	case TKN_OPERATOR_OR_OR:
	{
		if (TypeGetID(lhs) == ID_COLT_LSTRING || TypeGetID(rhs) == ID_COLT_LSTRING)
		{
			ast_gen_error(ast, line_nb, line, lexeme, "'%.*s' cannot have an 'lstring' as operand!", (uint32_t)(lexeme.end - lexeme.start), lexeme.start);
			Type ret = { .is_const = false, .typeinfo = &ColtVoid };
			return ret;
		}
	} //fall through on purpose
	case TKN_OPERATOR_EQUAL_EQUAL:
	case TKN_OPERATOR_BANG_EQUAL:
	{
		Type ret = { .is_const = false, .typeinfo = &ColtBool };
		return ret;
	}

	default:
		colt_unreachable("Invalid token");
	}
}

uint8_t ast_op_precedence(Token token)
{
	static const uint8_t operator_precedence_table[] =
	{
		10, 13, 0,	// +
		10, 13, 0,	// -
		11, 0,		// *
		11, 0,		// /
		8, 9, 8, 0,	// <<
		8, 9, 8, 0, // >>
		0, 7,		// =
		12, 7,		// !
		6, 0, 3,	// &
		4, 0, 2,	// |
		5, 0,		// ^
		12,			// ~
		11, 0		// %
	};
	colt_assert(token >= 0, "Token should be greater or equal to 0!");
	if (token < TKN_OPERATOR_LESS_COLON)
		return operator_precedence_table[token];
	return UINT8_MAX;
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
	while (ast->current_tkn != TKN_RIGHT_CURLY && ast->current_tkn != TKN_RIGHT_PAREN && ast->current_tkn != TKN_EOF && ast->current_tkn != TKN_SEMICOLON)
		ast->current_tkn = ScannerGetNextToken(&ast->scan);
}