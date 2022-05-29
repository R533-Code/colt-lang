/** @file expr.c
* Contains the definitions of the functions declared in 'expr.h'
*/
#include "ast.h"

Expr* make_literal_expr(QWORD value, Type type)
{
	LiteralExpr* ptr = safe_malloc(sizeof(LiteralExpr));
	//type for casting
	ptr->identifier = EXPR_LITERAL;
	
	ptr->value = value;
	ptr->expr_type = type;

	DO_IF_DEBUG_BUILD(
		switch (type.type_id)
		{
		case COLTI_BOOL:
		case COLTI_FLOAT:
		case COLTI_DOUBLE:
		case COLTI_INT8:
		case COLTI_INT16:
		case COLTI_INT32:
		case COLTI_INT64:
		case COLTI_UINT8:
		case COLTI_UINT16:
		case COLTI_UINT32:
		case COLTI_UINT64:
		break; default:
			colti_assert(false, "'type' was not a valid type!");
		}
	);

	return (Expr*)ptr;
}

Expr* make_unary_expr(Token unary_operator, Expr* child)
{
	UnaryExpr* ptr = safe_malloc(sizeof(UnaryExpr));
	//type for casting
	ptr->identifier = EXPR_UNARY;
	
	ptr->expr_type = child->expr_type;
	ptr->child = child;
	
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

Expr* make_binary_expr(Expr* lhs, Token binary_operator, Expr* rhs)
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

	return (Expr*)ptr;
}

Type impl_operator_type(Type lhs, Token binary_operator, Type rhs)
{
	//AT LEAST ONE IS NOT BUILT-IN TYPES
	if (lhs.type_id > COLTI_DOUBLE || rhs.type_id > COLTI_DOUBLE)
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
	colti_assert(lhs.type_id <= COLTI_DOUBLE && rhs.type_id <= COLTI_DOUBLE, "Type should be built-in types!");
	if (lhs.type_id > rhs.type_id)
		//swap so that the lhs has the lowest type
		return impl_builtin_inter_type(rhs, lhs);

	if (impl_is_type_int((OperandType)rhs.type_id
		&& impl_is_type_uint((OperandType)lhs.type_id)))
	{
		//We return the biggest integer be it signed or unsigned
		//The + 4 comes from the fact that unsigned integer ID is - 4
		//from that of signed integer ID
		return (lhs.type_id + 4 < rhs.type_id) ? rhs : lhs;
	}
	else //We return the greater type
		return rhs;
}

bool impl_is_type_int(OperandType type)
{
	return type < COLTI_FLOAT && type > COLTI_UINT64;
}

bool impl_is_type_uint(OperandType type)
{
	return type < COLTI_INT8 && type > COLTI_BOOL;
}

bool impl_is_type_floating(OperandType type)
{
	return type == COLTI_FLOAT || type == COLTI_DOUBLE;
}