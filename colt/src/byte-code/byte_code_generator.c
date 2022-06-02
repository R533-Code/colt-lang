/** @file byte_code_generator.c
* Contains the definitions of the functions in 'byte_code_generator.h'
*/
#include "byte_code_generator.h"

bool generateByteCode(Chunk* chunk, const Expr* expr)
{
	if (impl_gen_byte_code(chunk, expr))
	{
		//FOR DEBUG PURPOSE
		ChunkWriteOpCode(chunk, OP_PRINT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)expr->expr_type.type_id);
		//EXIT
		ChunkWriteOpCode(chunk, OP_EXIT);
		QWORD exit_code = { .u64 = 0 };
		ChunkWriteQWORD(chunk, exit_code);
		return true;
	}
	return false;
}

/*************************************
IMPLEMENTATION HELPERS
*************************************/

bool impl_gen_byte_code(Chunk* chunk, const Expr* expr)
{
	colt_assert(expr != NULL, "Generation should never happen if AST was not valid!");
	switch (expr->identifier)
	{
	break; case EXPR_UNARY:
		return impl_gen_code_unary(chunk, (const UnaryExpr*)expr);
	break; case EXPR_BINARY:
		return impl_gen_code_binary(chunk, (const BinaryExpr*)expr);
	break; case EXPR_LITERAL:
		return impl_gen_code_literal(chunk, (const LiteralExpr*)expr);
	break; case EXPR_CONVERT:
		return impl_gen_code_convert(chunk, (const ConvertExpr*)expr);
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
		colt_assert(false, "Operator was not unary!");
		return false;
	}
	return true;
}

bool impl_gen_code_binary(Chunk* chunk, const BinaryExpr* ptr)
{
	impl_gen_byte_code(chunk, ptr->lhs);
	impl_gen_byte_code(chunk, ptr->rhs);
	colt_assert(ptr->expr_type.type_id <= ID_COLT_DOUBLE, "Type ID should be of that of a built-in type!");
	switch (ptr->expr_operator)
	{
	case TKN_OPERATOR_PLUS:
		ChunkWriteOpCode(chunk, OP_ADD);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_MINUS:
		ChunkWriteOpCode(chunk, OP_SUBTRACT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_STAR:
		ChunkWriteOpCode(chunk, OP_MULTIPLY);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_SLASH:
		//prohibit zero division
		if (is_type_integral((BuiltinTypeID)ptr->expr_type.type_id))
		{
			ChunkWriteOpCode(chunk, OP_IMMEDIATE_QWORD);
			QWORD zero = { .u64 = 0 };
			ChunkWriteQWORD(chunk, zero);
			ChunkWriteOpCode(chunk, OP_SJUMP_NOT_EQUAL);
			ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
			uint64_t offset_pos = chunk->count;
			//to override by the jump offset
			ChunkWriteOpCode(chunk, OP_RETURN);
			ChunkWriteOpCode(chunk, OP_EXIT);
			QWORD exit_code = { .u64 = 1 };
			ChunkWriteQWORD(chunk, exit_code);
			chunk->code[offset_pos] = (uint8_t)(chunk->count - offset_pos);
			ChunkWriteOpCode(chunk, OP_POP);
		}
		ChunkWriteOpCode(chunk, OP_DIVIDE);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_AND:
		ChunkWriteOpCode(chunk, OP_BIT_AND);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_OR:
		ChunkWriteOpCode(chunk, OP_BIT_OR);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_XOR:
		ChunkWriteOpCode(chunk, OP_BIT_XOR);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_TILDE:
		ChunkWriteOpCode(chunk, OP_BIT_NOT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_GREATER_GREATER:
		ChunkWriteOpCode(chunk, OP_BIT_SHIFT_R);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_LESS_LESS:
		ChunkWriteOpCode(chunk, OP_BIT_SHIFT_L);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_GREATER:
		ChunkWriteOpCode(chunk, OP_CMP_GREATER);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_GREATER_EQUAL:
		ChunkWriteOpCode(chunk, OP_CMP_GREATER_EQ);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_LESS:
		ChunkWriteOpCode(chunk, OP_CMP_LESS);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_LESS_EQUAL:
		ChunkWriteOpCode(chunk, OP_CMP_LESS_EQ);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_EQUAL_EQUAL:
		ChunkWriteOpCode(chunk, OP_CMP_EQUAL);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	case TKN_OPERATOR_BANG_EQUAL:
		ChunkWriteOpCode(chunk, OP_CMP_NOT_EQUAL);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	default:
		colt_assert(false, "NOT IMPLEMENTED!");
		return false;
	}
}

bool impl_gen_code_literal(Chunk* chunk, const LiteralExpr* ptr)
{
	switch (ptr->expr_type.type_id)
	{
	case ID_COLT_I8:
	case ID_COLT_U8:
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
		colt_assert(false, "Type ID should be of that of a built-in type!");
		return false;
	}
	return true;
}

bool impl_gen_code_convert(Chunk* chunk, const ConvertExpr* ptr)
{
	impl_gen_byte_code(chunk, ptr->child);
	ChunkWriteOpCode(chunk, OP_CONVERT);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->child->expr_type.type_id);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);	
	return true;
}
