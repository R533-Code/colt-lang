#include "byte_code_generator.h"

bool generateByteCode(Chunk* chunk, const Expr* expr)
{
	if (impl_gen_byte_code(chunk, expr))
	{
		//FOR DEBUG PURPOSE
		ChunkWriteOpCode(chunk, OP_PRINT);
		ChunkWriteOperand(chunk, expr->expr_type.type_id);
		//EXIT
		ChunkWriteOpCode(chunk, OP_RETURN);
		return true;
	}
	return false;
}

/*************************************
IMPLEMENTATION HELPERS
*************************************/

bool impl_gen_byte_code(Chunk* chunk, const Expr* expr)
{
	switch (expr->identifier)
	{
	break; case EXPR_UNARY:
		return impl_gen_code_unary(chunk, (const UnaryExpr*)expr);
	break; case EXPR_BINARY:
		return impl_gen_code_binary(chunk, (const BinaryExpr*)expr);
	break; case EXPR_LITERAL:
		return impl_gen_code_literal(chunk, (const LiteralExpr*)expr);
	}
	return true;
}

bool impl_gen_code_unary(Chunk* chunk, const UnaryExpr* ptr)
{
	//TODO: should use Type to actually dispatch call depending on operator
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_MINUS:
		impl_gen_byte_code(chunk, ptr->child);
		ChunkWriteOpCode(chunk, OP_NEGATE);
		ChunkWriteOperand(chunk, ptr->expr_type.type_id);
	break; case TKN_OPERATOR_PLUS:
	break; case TKN_OPERATOR_BANG:
	break; case TKN_OPERATOR_TILDE:
		//DOES NOT DO ANYTHING
	break; default:
		colti_assert(false, "Operator was not unary!");
		return false;
	}
	return true;
}

bool impl_gen_code_binary(Chunk* chunk, const BinaryExpr* ptr)
{
	impl_gen_byte_code(chunk, ptr->lhs);
	impl_gen_byte_code(chunk, ptr->rhs);
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_PLUS:
		ChunkWriteOpCode(chunk, OP_ADD);
		ChunkWriteOperand(chunk, ptr->expr_type.type_id);
	break; case TKN_OPERATOR_MINUS:
		ChunkWriteOpCode(chunk, OP_SUBTRACT);
		ChunkWriteOperand(chunk, ptr->expr_type.type_id);
	break; case TKN_OPERATOR_STAR:
		ChunkWriteOpCode(chunk, OP_MULTIPLY);
		ChunkWriteOperand(chunk, ptr->expr_type.type_id);
	break; case TKN_OPERATOR_SLASH:
		ChunkWriteOpCode(chunk, OP_DIVIDE);
		ChunkWriteOperand(chunk, ptr->expr_type.type_id);
	break; default:
		colti_assert(false, "NOT IMPLEMENTED!");
		return false;
	}
	return true;
}

bool impl_gen_code_literal(Chunk* chunk, const LiteralExpr* ptr)
{
	switch (ptr->expr_type.type_id)
	{
	case ID_COLT_BOOL:
		ChunkWriteOpCode(chunk, OP_IMMEDIATE_BYTE);
		ChunkWriteBYTE(chunk, ptr->value.byte);
		break;
	case ID_COLT_DOUBLE:
	case ID_COLT_U64:
	case ID_COLT_I64:
		ChunkWriteOpCode(chunk, OP_IMMEDIATE_QWORD);
		ChunkWriteQWORD(chunk, ptr->value);
		break;
	case ID_COLT_FLOAT:
	case ID_COLT_I32:
	case ID_COLT_U32:
		ChunkWriteOpCode(chunk, OP_IMMEDIATE_DWORD);
		ChunkWriteDWORD(chunk, ptr->value.dword);
		break;
	case ID_COLT_I16:
	case ID_COLT_U16:
		ChunkWriteOpCode(chunk, OP_IMMEDIATE_WORD);
		ChunkWriteWORD(chunk, ptr->value.word);
		break;

	default:
		colti_assert(false, "Type ID should be of that of a built-in type!");
		return false;
	}
	return true;
}