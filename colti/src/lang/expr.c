/** @file expr.c
* Contains the definitions of the functions declared in 'expr.h'
*/
#include "ast.h"

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
		switch (type.type_id)
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
		break; default:
			colti_assert(false, "'type' was not a valid type!");
		}
	);

	return (Expr*)ptr;
}

Expr* makeUnaryExpr(Token unary_operator, Expr* child, uint64_t line_nb, StringView line, StringView lexeme)
{
	UnaryExpr* ptr = safe_malloc(sizeof(UnaryExpr));
	//type for casting
	ptr->identifier = EXPR_UNARY;
	
	ptr->expr_type = child->expr_type;
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

		break; default:
			colti_assert(false, "'unary_operator' was not a valid unary operator!");
		}
	);

	ptr->expr_operator = unary_operator;
	return (Expr*)ptr;
}

Expr* makeBinaryExpr(Expr* lhs, Token binary_operator, Expr* rhs, uint64_t line_nb, StringView line, StringView lexeme)
{
	BinaryExpr* ptr = safe_malloc(sizeof(BinaryExpr));
	//type for casting
	ptr->identifier = EXPR_BINARY;
	
	ptr->expr_operator = binary_operator;

	// get the type of the operator knowing the type of its operands
	// This does not convert any of the values but rather choose the whole expression's type
	ptr->expr_type = impl_operator_type(lhs->expr_type, binary_operator, rhs->expr_type);

	ptr->lhs = lhs;
	ptr->rhs = rhs;

	ptr->line_nb = line_nb;
	ptr->line = line;
	ptr->lexeme = lexeme;

	return (Expr*)ptr;
}

void freeExpr(Expr* ptr)
{
	if (ptr == NULL)
		return;

	switch (ptr->identifier)
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
	break; case EXPR_LITERAL:
		safe_free(ptr);
	break; default:
		colti_assert(false, "Expression identifier was invalid!");
	}
}

/*******************************
IMPLEMENTATION HELPER
*******************************/

Type impl_operator_type(Type lhs, Token binary_operator, Type rhs)
{
	//AT LEAST ONE IS NOT BUILT-IN TYPES
	if (lhs.type_id > ID_COLT_DOUBLE || rhs.type_id > ID_COLT_DOUBLE)
	{
		//Check operator overloads table
		colti_assert(false, "NOT IMPLEMENTED YET");
	}
	else if (lhs.type_id != rhs.type_id)
	{
		return impl_builtin_inter_type(lhs, rhs);
	}	
	else //if both are the same
		return lhs;
}

Type impl_builtin_inter_type(Type lhs, Type rhs)
{
	colti_assert(lhs.type_id <= ID_COLT_DOUBLE && rhs.type_id <= ID_COLT_DOUBLE, "Type should be built-in types!");
	if (lhs.type_id > rhs.type_id)
		//swap so that the lhs has the lowest type
		return impl_builtin_inter_type(rhs, lhs);

	if (impl_is_type_int((BuiltinTypeID)rhs.type_id
		&& impl_is_type_uint((BuiltinTypeID)lhs.type_id)))
	{
		//We return the biggest integer be it signed or unsigned
		//The + 4 comes from the fact that unsigned integer ID is - 4
		//from that of signed integer ID
		return (lhs.type_id + 4 < rhs.type_id) ? rhs : lhs;
	}
	else //We return the greater type
		return rhs;
}

bool impl_is_type_int(BuiltinTypeID type)
{
	return type < ID_COLT_FLOAT && type > ID_COLT_U64;
}

bool impl_is_type_uint(BuiltinTypeID type)
{
	return type < ID_COLT_I8 && type > ID_COLT_BOOL;
}

bool impl_is_type_floating(BuiltinTypeID type)
{
	return type == ID_COLT_FLOAT || type == ID_COLT_DOUBLE;
}