/** @file expr.c
* Contains the definitions of the functions declared in 'expr.h'
*/
#include "ast.h"

const Expr* expr_array_front(const ExprArray* array)
{
	colt_assert(array->count != 0, "Array was empty!");
	return array->expressions[0];
}

const Expr* expr_array_back(const ExprArray* array)
{
	colt_assert(array->count != 0, "Array was empty!");
	return array->expressions[array->count - 1];
}

void ExprArrayInit(ExprArray* array)
{
	array->capacity = 10;
	array->count = 0;
	array->expressions = safe_malloc(array->capacity * sizeof(Expr*));
}

void ExprArrayFree(ExprArray* array)
{
	for (size_t i = 0; i < array->count; i++)
		freeExpr(array->expressions[i]);
	safe_free(array->expressions);
	DO_IF_DEBUG_BUILD(array->count = 0);
	DO_IF_DEBUG_BUILD(array->capacity = 0);
}

void ExprArrayClear(ExprArray* array)
{
	for (size_t i = 0; i < array->count; i++)
		freeExpr(array->expressions[i]);
	array->count = 0;
}

void ExprArrayPushBack(ExprArray* array, Expr* expr)
{
	//handle resize
	if (array->count == array->capacity)
	{
		array->capacity *= 2;
		Expr** ptr = safe_malloc(array->capacity * sizeof(Expr*));
		memcpy(ptr, array->expressions, array->count * sizeof(Expr*));
		safe_free(array->expressions);
		array->expressions = ptr;
	}
	array->expressions[array->count++] = expr;
}

uint64_t ScopeExprGetOffset(ScopeExpr* scope)
{
	if (scope == NULL)
		return 0;
	else if (scope->parent_scope == NULL)
		return 0;
	return scope->parent_scope->var_count + ScopeExprGetOffset(scope->parent_scope->parent_scope);
}
bool ScopeExprIsVarDeclared(ScopeExpr* scope, StringView name)
{
	if (scope == NULL)
		return false;

	Expr** expr_ptr = scope->array.expressions;
	for (size_t i = 0; i < scope->array.count; i++)
	{
		if (expr_ptr[i]->identifier == EXPR_LOCAL_READ || expr_ptr[i]->identifier == EXPR_LOCAL_WRITE)
			if (StringViewEqual(((LocalReadExpr*)expr_ptr[i])->var_name, name))
				return true;
	}
	if (scope->parent_scope == NULL)
		return false;
	//recurse into the parent directory
	return ScopeExprIsVarDeclared(scope->parent_scope, name);
}

Expr* ScopeExprFindVar(ScopeExpr* scope, StringView name)
{
	if (scope == NULL)
		return NULL;

	Expr** expr_ptr = scope->array.expressions;
	for (size_t i = 0; i < scope->array.count; i++)
	{
		if (expr_ptr[i]->identifier == EXPR_LOCAL_READ || expr_ptr[i]->identifier == EXPR_LOCAL_WRITE)
			if (StringViewEqual(((LocalReadExpr*)expr_ptr[i])->var_name, name))
				return expr_ptr[i];
	}
	if (scope->parent_scope == NULL)
		return NULL; //not found, which means it can be a global or simply not exists
	//recurse into the parent directory
	return ScopeExprFindVar(scope->parent_scope, name);
}

Expr* makeLiteralExpr(QWORD value, Type type, uint64_t line_nb, StringView line, StringView lexeme)
{
	LiteralExpr* ptr = safe_malloc(sizeof(LiteralExpr));
	//type for casting
	ptr->identifier = EXPR_LITERAL;
	
	ptr->value = value;
	ptr->expr_type = type;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	DO_IF_DEBUG_BUILD(
		switch (TypeGetID(type))
		{
		case ID_COLT_BOOL:
		case ID_COLT_FLOAT:
		case ID_COLT_DOUBLE:
		case ID_COLT_I8:
		case ID_COLT_I16:
		case ID_COLT_I32:
		case ID_COLT_I64:
		case ID_COLT_U8:
		case ID_COLT_U16:
		case ID_COLT_U32:
		case ID_COLT_U64:
		case ID_COLT_LSTRING:
		case ID_COLT_CHAR:
		break; default:
			colt_unreachable("'type' was not a valid type!");
		}
	);

	return (Expr*)ptr;
}

Expr* makeUnaryExpr(Token unary_operator, Expr* child, Type type, uint64_t line_nb, StringView line, StringView lexeme)
{
	UnaryExpr* ptr = safe_malloc(sizeof(UnaryExpr));
	//type for casting
	ptr->identifier = EXPR_UNARY;
	
	ptr->expr_type = type;
	ptr->child = child;
	
	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	DO_IF_DEBUG_BUILD(
		switch (unary_operator)
		{
		case TKN_OPERATOR_TILDE:
		case TKN_OPERATOR_BANG:
		case TKN_OPERATOR_PLUS:
		case TKN_OPERATOR_MINUS:
		case TKN_KEYWORD_STATIC_PRINT:
		break; default:
			colt_unreachable("'unary_operator' was not a valid unary operator!");
		}
	);

	ptr->expr_operator = unary_operator;
	return (Expr*)ptr;
}

Expr* makeBinaryExpr(Expr* lhs, Token binary_operator, Expr* rhs, Type expr_type, uint64_t line_nb, StringView line, StringView lexeme)
{
	BinaryExpr* ptr = safe_malloc(sizeof(BinaryExpr));
	//type for casting
	ptr->identifier = EXPR_BINARY;
	
	ptr->expr_operator = binary_operator;
	ptr->expr_type = expr_type;

	ptr->lhs = lhs;
	ptr->rhs = rhs;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeConvertExpr(Expr* expr, Type convert_to, uint64_t line_nb, StringView line, StringView lexeme)
{
	if (TypeEqualTypeID(convert_to, TypeGetID(expr->expr_type)))
		return expr;

	ConvertExpr* ptr = safe_malloc(sizeof(ConvertExpr));
	//type for casting
	ptr->identifier = EXPR_CONVERT;
	
	ptr->expr_type = convert_to;
	ptr->child = expr;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeConditionExpr()
{
	ConditionExpr* ptr = safe_malloc(sizeof(ConditionExpr));
	
	ptr->identifier = EXPR_CONDITION;
	ExprArrayInit(&ptr->elif_executes);
	ExprArrayInit(&ptr->elif_conditions);

	ptr->else_execute = NULL;
	
	ptr->expr_type.is_const = false;
	ptr->expr_type.typeinfo = &ColtVoid;

	return (Expr*)ptr;
}

Expr* makeGlobalReadExpr(StringView var_name, Type var_type, uint64_t line_nb, StringView line, StringView lexeme)
{
	GlobalReadExpr* ptr = safe_malloc(sizeof(GlobalReadExpr));

	ptr->identifier = EXPR_GLOB_READ;

	ptr->expr_type = var_type;

	ptr->var_name = var_name;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeGlobalWriteExpr(StringView var_name, Type var_type,  Expr* value, uint64_t line_nb, StringView line, StringView lexeme)
{
	GlobalWriteExpr* ptr = safe_malloc(sizeof(GlobalWriteExpr));

	ptr->identifier = EXPR_GLOB_WRITE;
	ptr->expr_type = var_type;
	ptr->var_name = var_name;
	ptr->value = value;
	
	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeLocalReadExpr(StringView var_name, Type var_type, uint64_t var_offset, uint64_t line_nb, StringView line, StringView lexeme)
{
	LocalReadExpr* ptr = safe_malloc(sizeof(LocalReadExpr));

	ptr->identifier = EXPR_LOCAL_READ;
	ptr->expr_type = var_type;
	ptr->offset = var_offset;
	ptr->var_name = var_name;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeLocalWriteExpr(StringView var_name, Type var_type, uint64_t var_offset, Expr* value, uint64_t line_nb, StringView line, StringView lexeme)
{
	LocalWriteExpr* ptr = safe_malloc(sizeof(LocalWriteExpr));

	ptr->identifier = EXPR_LOCAL_WRITE;
	ptr->expr_type = var_type;
	ptr->offset = var_offset;
	ptr->var_name = var_name;
	ptr->value = value;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

Expr* makeScopeExpr(ScopeExpr* parent_scope)
{
	ScopeExpr* ptr = safe_malloc(sizeof(ScopeExpr));

	ptr->identifier = EXPR_SCOPE;
	ptr->expr_type.is_const = false;
	ptr->expr_type.typeinfo = &ColtVoid;
	ExprArrayInit(&ptr->array);
	ptr->var_count = 0;
	ptr->parent_scope = parent_scope;

	return (Expr*)ptr;
}

void freeExpr(Expr* ptr)
{
	if (ptr == NULL)
		return;

	switch (ptr->identifier)
	{
	break; case EXPR_FN:
	{
		//TODO: when implemented
	}
	break; case EXPR_UNARY:
	{
		UnaryExpr* uexpr = (UnaryExpr*)ptr;
		//we need to recurse to free expressions of expressions
		freeExpr(uexpr->child);
		safe_free(uexpr);
	}
	break; case EXPR_BINARY:
	{
		BinaryExpr* bexpr = (BinaryExpr*)ptr;
		//we need to recurse to free expressions of expressions
		freeExpr(bexpr->lhs);
		freeExpr(bexpr->rhs);
		safe_free(bexpr);
	}
	break; case EXPR_CONVERT:
	{
		ConvertExpr* cexpr = (ConvertExpr*)ptr;
		//we need to recurse to free expressions of expressions
		freeExpr(cexpr->child);
		safe_free(cexpr);
	}
	break; case EXPR_LITERAL:
	{
		LiteralExpr* lexpr = (LiteralExpr*)ptr;
		if (TypeEqualTypeID(ptr->expr_type, ID_COLT_LSTRING))
		{
			StringFree(lexpr->value.string_ptr);
			safe_free(lexpr->value.string_ptr);
		}
		safe_free(ptr);
	}
	break; case EXPR_SCOPE:
	{
		ScopeExpr* scexpr = (ScopeExpr*)ptr;
		//Free each expression in the scope
		ExprArrayFree(&scexpr->array);
		safe_free(ptr);
	}
	break; case EXPR_CONDITION:
	{
		ConditionExpr* cexpr = (ConditionExpr*)ptr;

		freeExpr(cexpr->if_condition);
		freeExpr(cexpr->if_execute);

		if (cexpr->else_execute != NULL)
			freeExpr(cexpr->else_execute);

		//Free each expression in the scope
		ExprArrayFree(&cexpr->elif_conditions);
		ExprArrayFree(&cexpr->elif_executes);
		safe_free(ptr);
	}
	break; case EXPR_GLOB_WRITE:
	{
		GlobalWriteExpr* rexpr = (GlobalWriteExpr*)ptr;
		freeExpr(rexpr->value);
		safe_free(ptr);
	}
	break; case EXPR_LOCAL_WRITE:
	{
		LocalWriteExpr* rexpr = (LocalWriteExpr*)ptr;
		freeExpr(rexpr->value);
		safe_free(ptr);
	}
	break;
	case EXPR_GLOB_READ:
	case EXPR_LOCAL_READ:
		safe_free(ptr);
	break; default:
		colt_unreachable("Expression identifier was invalid!");
	}
}

/*******************************
IMPLEMENTATION HELPER
*******************************/

Type builtin_inter_type(Type lhs, Type rhs)
{
	colt_assert(TypeGetID(lhs) <= ID_COLT_DOUBLE && TypeGetID(rhs) <= ID_COLT_DOUBLE, "Type should be built-in types!");

	uint64_t real_lhs = is_type_signed_int(lhs) ? TypeGetID(lhs) - 4 : TypeGetID(lhs);
	uint64_t real_rhs = is_type_signed_int(rhs) ? TypeGetID(rhs) - 4 : TypeGetID(rhs);
	if (real_lhs > real_rhs)		
		return builtin_inter_type(rhs, lhs); //swap so that the 'lhs' has the lowest type

	if (is_type_signed_int(rhs) && is_type_unsigned_int(lhs))
	{
		//We return the biggest integer be it signed or unsigned
		//The + 4 comes from the fact that unsigned integer ID is - 4
		//from that of signed integer ID
		return (TypeGetID(lhs) + 4 < TypeGetID(rhs)) ? rhs : lhs;
	}
	else //We return the greater type
		return rhs;
}