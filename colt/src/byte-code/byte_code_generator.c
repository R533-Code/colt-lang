/** @file byte_code_generator.c
* Contains the definitions of the functions in 'byte_code_generator.h'
*/
#include "byte_code_generator.h"

bool generateByteCode(Chunk* chunk, const VariableTable* var_table, const Expr* expr)
{
	colt_assert(chunk->count >= 32, "Chunk should be initialized!");

	//write GLOBAL offset
	*(uint64_t*)(chunk->code) = gen_global_pool(chunk, var_table);
	//*((uint64_t*)(chunk->code) + 1) = gen_const_pool(chunk, var_table);
	//write DEBUG offset
	*((uint64_t*)(chunk->code) + 2) = gen_debug_pool(chunk, var_table);
	//write CODE offset
	*((uint64_t*)(chunk->code) + 3) = chunk->count;

	if (gen_byte_code(chunk, var_table, expr))
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

uint64_t gen_global_pool(Chunk* chunk, const VariableTable* var_table)
{
	if (var_table->count == 0)
		return 0;
	uint64_t global_begin = chunk->count;

	for (size_t i = 0; i < var_table->capacity; i++)
	{
		//not active entry
		if (var_table->entries[i].key.ptr == NULL)
			continue;
		*((QWORD*)(chunk->code + global_begin) + var_table->entries[i].counter_nb) = var_table->entries[i].value;
		chunk->count += sizeof(QWORD);
	}
	return global_begin;
}

uint64_t gen_debug_pool(Chunk* chunk, const VariableTable* var_table)
{
	if (var_table->count == 0)
		return 0;
	uint64_t debug_begin = chunk->count;


	for (size_t i = 0; i < var_table->capacity; i++)
	{
		//not active entry
		if (var_table->entries[i].key.ptr == NULL)
			continue;
		ChunkWriteOperand(chunk, (BuiltinTypeID)var_table->entries[i].type.type_id);
	}
	return debug_begin;
}

/*************************************
IMPLEMENTATION HELPERS
*************************************/

bool gen_byte_code(Chunk* chunk, const VariableTable* var_table, const Expr* expr)
{
	colt_assert(expr != NULL, "Generation should never happen if AST was not valid!");
	switch (expr->identifier)
	{
	break; case EXPR_UNARY:
		return impl_gen_code_unary(chunk, var_table, (const UnaryExpr*)expr);
	break; case EXPR_BINARY:
		return impl_gen_code_binary(chunk, var_table, (const BinaryExpr*)expr);
	break; case EXPR_LITERAL:
		return impl_gen_code_literal(chunk, (const LiteralExpr*)expr);
	break; case EXPR_CONVERT:
		return impl_gen_code_convert(chunk, var_table, (const ConvertExpr*)expr);
	break; case EXPR_VAR:
		return impl_gen_global_code_variable(chunk, var_table, (const VariableExpr*)expr);
	break; default:
		colt_assert(false, "NOT IMPLEMENTED YET!");
	}
	return true;
}

bool impl_gen_code_unary(Chunk* chunk, const VariableTable* var_table, const UnaryExpr* ptr)
{
	//TODO: should use Type to actually dispatch call depending on operator
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_MINUS:
		gen_byte_code(chunk, var_table, ptr->child);
		ChunkWriteOpCode(chunk, OP_NEGATE);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
	break; case TKN_OPERATOR_PLUS:
		//DOES NOT DO ANYTHING
	break; case TKN_OPERATOR_BANG:
		gen_byte_code(chunk, var_table, ptr->child);
		//ChunkWriteOpCode(chunk, )
	break; case TKN_OPERATOR_TILDE:
		gen_byte_code(chunk, var_table, ptr->child);
		ChunkWriteOpCode(chunk, OP_BIT_NOT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
	break; default:
		colt_assert(false, "Operator was not unary!");
		return false;
	}
	return true;
}

bool impl_gen_code_binary(Chunk* chunk, const VariableTable* var_table, const BinaryExpr* ptr)
{
	if (ptr->expr_operator == TKN_OPERATOR_EQUAL)
		return gen_global_variable_assigment(chunk, var_table, ptr);

	gen_byte_code(chunk, var_table, ptr->lhs);
	gen_byte_code(chunk, var_table, ptr->rhs);
	colt_assert(ptr->expr_type.type_id <= ID_COLT_DOUBLE, "Type ID should be of that of a built-in type!");
	switch (ptr->expr_operator)
	{
	case TKN_OPERATOR_PLUS:
		if (is_type_signed_int(ptr->expr_type.type_id))
			gen_signed_addition_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		ChunkWriteOpCode(chunk, OP_ADD);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_MINUS:
		if (is_type_signed_int(ptr->expr_type.type_id))
			gen_signed_subtraction_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		ChunkWriteOpCode(chunk, OP_SUBTRACT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_STAR:
		if (is_type_signed_int(ptr->expr_type.type_id))
			gen_signed_multiplication_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		ChunkWriteOpCode(chunk, OP_MULTIPLY);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_SLASH:
		if (is_type_signed_int(ptr->expr_type.type_id))
			gen_signed_division_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		//prohibit zero division for integers
		if (is_type_integral((BuiltinTypeID)ptr->expr_type.type_id))
		{
			QWORD zero = { .u64 = 0 };
			gen_integral_short_jmp(chunk, OP_SJUMP_NOT_EQUAL, zero, (BuiltinTypeID)ptr->expr_type.type_id);
		}
		ChunkWriteOpCode(chunk, OP_DIVIDE);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;
	case TKN_OPERATOR_MODULO:
		//prohibit mod 0
		if (is_type_integral((BuiltinTypeID)ptr->expr_type.type_id))
		{
			QWORD zero = { .u64 = 0 };
			gen_integral_short_jmp(chunk, OP_SJUMP_NOT_EQUAL, zero, (BuiltinTypeID)ptr->expr_type.type_id);
		}
		ChunkWriteOpCode(chunk, OP_MODULO);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
		return true;

		/****************************************
		* BITWISE OPCODES
		****************************************/

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
	{
		gen_bitshift_ub_checks(chunk, ptr->expr_type);
		ChunkWriteOpCode(chunk, OP_BIT_SHIFT_R);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	}
	case TKN_OPERATOR_LESS_LESS:
	{
		gen_bitshift_ub_checks(chunk, ptr->expr_type);
		ChunkWriteOpCode(chunk, OP_BIT_SHIFT_L);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id);
		return true;
	}
	
		/****************************************
		* COMPARISON OPCODES
		****************************************/

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
		ChunkWriteOpCode(chunk, OP_PUSH_BYTE);
		ChunkWriteBYTE(chunk, ptr->value.byte);
		break;
	case ID_COLT_DOUBLE:
	case ID_COLT_U64:
	case ID_COLT_I64:
		ChunkWriteOpCode(chunk, OP_PUSH_QWORD);
		ChunkWriteQWORD(chunk, ptr->value);
		break;
	case ID_COLT_FLOAT:
	case ID_COLT_I32:
	case ID_COLT_U32:
		ChunkWriteOpCode(chunk, OP_PUSH_DWORD);
		ChunkWriteDWORD(chunk, ptr->value.dword);
		break;
	case ID_COLT_I16:
	case ID_COLT_U16:
		ChunkWriteOpCode(chunk, OP_PUSH_WORD);
		ChunkWriteWORD(chunk, ptr->value.word);
		break;
	default:
		colt_assert(false, "Type ID should be of that of a built-in type!");
		return false;
	}
	return true;
}

bool impl_gen_code_convert(Chunk* chunk, const VariableTable* var_table, const ConvertExpr* ptr)
{
	gen_byte_code(chunk, var_table, ptr->child);
	ChunkWriteOpCode(chunk, OP_CONVERT);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->child->expr_type.type_id);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);	
	return true;
}

bool impl_gen_global_code_variable(Chunk* chunk, const VariableTable* var_table, const VariableExpr* ptr)
{
	VariableEntry* entry = table_find_entry(var_table->entries, var_table->capacity, ptr->var_name);
	
	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	QWORD offset = { .u64 = entry->counter_nb };
	
	switch (ptr->expr_type.byte_size)
	{
	break; case 1:
		ChunkWriteOpCode(chunk, OP_LOAD_BYTE);
	break; case 2:
		ChunkWriteOpCode(chunk, OP_LOAD_WORD);
	break; case 4:
		ChunkWriteOpCode(chunk, OP_LOAD_DWORD);
	break; case 8:
		ChunkWriteOpCode(chunk, OP_LOAD_QWORD);
	break; default:
		colt_assert(false, "Unexpected operand for LOAD!");
	}
	ChunkWriteQWORD(chunk, offset);
	return true;
}

bool gen_global_variable_assigment(Chunk* chunk, const VariableTable* var_table, const BinaryExpr* ptr)
{
	gen_byte_code(chunk, var_table, ptr->rhs);

	colt_assert(ptr->lhs->identifier == EXPR_VAR, "Left hand side should be a variable!");
	if (ptr->lhs->identifier != EXPR_VAR)
		return false;

	const VariableEntry* entry = table_find_entry(var_table->entries, var_table->capacity, ((VariableExpr*)ptr->lhs)->var_name);

	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	QWORD offset = { .u64 = entry->counter_nb };

	switch (ptr->lhs->expr_type.byte_size)
	{
	break; case 1:
		ChunkWriteOpCode(chunk, OP_STORE_BYTE);
	break; case 2:
		ChunkWriteOpCode(chunk, OP_STORE_WORD);
	break; case 4:
		ChunkWriteOpCode(chunk, OP_STORE_DWORD);
	break; case 8:
		ChunkWriteOpCode(chunk, OP_STORE_QWORD);
	break; default:
		colt_assert(false, "Unexpected operand for STORE!");
	}
	ChunkWriteQWORD(chunk, offset);
	return true;
}

void gen_integral_short_jmp(Chunk* chunk, OpCode short_jump, QWORD cmp_to, BuiltinTypeID type)
{
	//push the value to compare to
	ChunkWriteOpCode(chunk, OP_PUSH_QWORD);
	ChunkWriteQWORD(chunk, cmp_to);

	//write the short jump and the type that follows it
	ChunkWriteOpCode(chunk, short_jump);
	ChunkWriteOperand(chunk, type);

	//we store the offset where the jump offset should be written
	uint64_t offset_pos = chunk->count;
	//will be overridden by the jump offset
	ChunkWriteOpCode(chunk, OP_RETURN);
	ChunkWriteOpCode(chunk, OP_EXIT);
	QWORD exit_code = { .u64 = 1 };
	ChunkWriteQWORD(chunk, exit_code);
	
	//overwrite the OP_RETURN by the jump offset
	chunk->code[offset_pos] = (uint8_t)(chunk->count - offset_pos);
	//pop the value we compared with
	ChunkWriteOpCode(chunk, OP_POP);
}

void gen_bitshift_ub_checks(Chunk* chunk, Type type)
{
	QWORD zero = { .u64 = 0 };
	QWORD bit_size = { .u64 = 0 };
	switch (type.type_id)
	{
	case ID_COLT_U8:
	case ID_COLT_U16:
	case ID_COLT_U32:
	case ID_COLT_U64:
		bit_size.u64 = type.byte_size * 8;
	break; case ID_COLT_I8:
		bit_size.i8 = (int8_t)type.byte_size * 8;
	break; case ID_COLT_I16:
		bit_size.i16 = (int16_t)type.byte_size * 8;
	break; case ID_COLT_I32:
		bit_size.i32 = (int32_t)type.byte_size * 8;
	break; case ID_COLT_I64:
		bit_size.i64 = (int64_t)type.byte_size * 8;
	break; default:
		colt_assert(false, "Unexpected type for bit-shift operands!");
	}
	gen_integral_short_jmp(chunk, OP_SJUMP_GREATER_EQ, zero, (BuiltinTypeID)type.type_id);
	gen_integral_short_jmp(chunk, OP_SJUMP_LESS, bit_size, (BuiltinTypeID)type.type_id);
}

void gen_signed_addition_checks(Chunk* chunk, BuiltinTypeID type)
{

}

void gen_signed_subtraction_checks(Chunk* chunk, BuiltinTypeID type)
{

}

void gen_signed_multiplication_checks(Chunk* chunk, BuiltinTypeID type)
{

}

void gen_signed_division_checks(Chunk* chunk, BuiltinTypeID type)
{

}