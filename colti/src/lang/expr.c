#include "ast.h"

Expr* make_literal_expr(QWORD value, OperandType type)
{
	LiteralExpr* ptr = safe_malloc(sizeof(LiteralExpr));
	//type for casting
	ptr->identifier = EXPR_LITERAL;
	
	ptr->value = value;
	ptr->expr_type.type_id = type;

	DO_IF_DEBUG_BUILD(
		switch (type)
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
	//FIXME: should convert and fix types
	ptr->expr_type = lhs->expr_type;

	ptr->lhs = lhs;
	ptr->rhs = rhs;

	return ptr;
}
