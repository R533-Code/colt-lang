/** @file byte_code_generator.c
* Contains the definitions of the functions in 'byte_code_generator.h'
*/
#include "byte_code_generator.h"

/// @brief Generates the byte-code necessary for an addition, and its checks
#define gen_binary_plus()	do { if (is_type_signed_int(ptr->expr_type.type_id)) \
								gen_signed_addition_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id); \
							ChunkWriteOpCode(chunk, OP_ADD); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a subtraction, and its checks
#define gen_binary_minus() do { if (is_type_signed_int(ptr->expr_type.type_id)) \
								gen_signed_subtraction_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id); \
							ChunkWriteOpCode(chunk, OP_SUBTRACT); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a multiplication, and its checks
#define gen_binary_star() do { if (is_type_signed_int(ptr->expr_type.type_id)) \
								gen_signed_multiplication_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id); \
							ChunkWriteOpCode(chunk, OP_MULTIPLY); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a division, and its checks
#define gen_binary_slash() do { if (is_type_signed_int(ptr->expr_type.type_id)) \
									gen_signed_division_checks(chunk, (BuiltinTypeID)ptr->expr_type.type_id); \
								/*prohibit zero division for integers*/ \
								if (is_type_integral((BuiltinTypeID)ptr->expr_type.type_id)) \
								{ \
									QWORD zero = { .u64 = 0 }; \
									gen_integral_short_jmp(chunk, OP_SJUMP_NOT_EQUAL, zero, (BuiltinTypeID)ptr->expr_type.type_id); \
								} \
								ChunkWriteOpCode(chunk, OP_DIVIDE); \
								ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a modulo operation, and its checks
#define gen_binary_modulo() do { /*prohibit zero division for integers*/ \
								if (is_type_integral((BuiltinTypeID)ptr->expr_type.type_id)) \
								{ \
									QWORD zero = { .u64 = 0 }; \
									gen_integral_short_jmp(chunk, OP_SJUMP_NOT_EQUAL, zero, (BuiltinTypeID)ptr->expr_type.type_id); \
								} \
								ChunkWriteOpCode(chunk, OP_DIVIDE); \
								ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a bitwise AND, and its checks
#define gen_binary_and() do { colt_assert(is_type_integral(ptr->expr_type.type_id), "Type should be integral!"); \
							ChunkWriteOpCode(chunk, OP_BIT_AND); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a bitwise OR, and its checks
#define gen_binary_or() do { colt_assert(is_type_integral(ptr->expr_type.type_id), "Type should be integral!"); \
							ChunkWriteOpCode(chunk, OP_BIT_OR); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a bitwise XOR, and its checks
#define gen_binary_xor() do { colt_assert(is_type_integral(ptr->expr_type.type_id), "Type should be integral!"); \
							ChunkWriteOpCode(chunk, OP_BIT_XOR); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a bitwise left shift, and its checks
#define gen_binary_lshift() do { gen_bitshift_ub_checks(chunk, ptr->expr_type); \
							ChunkWriteOpCode(chunk, OP_BIT_SHIFT_L); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id); } while (0)

/// @brief Generates the byte-code necessary for a bitwise right shift, and its checks
#define gen_binary_rshift() do { gen_bitshift_ub_checks(chunk, ptr->expr_type); \
							ChunkWriteOpCode(chunk, OP_BIT_SHIFT_R); \
							ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->lhs->expr_type.type_id); } while (0)


bool generateByteCode(Chunk* chunk, const ASTTable* table, const ExprArray* array, bool print_last_expr)
{
	colt_assert(array->count != 0, "Cannot generate byte-code if expr == NULL!");
	colt_assert(chunk->count >= 40, "Chunk should be initialized!");
	
	//Reserve enough size for the GLOBAL/CONST and string literals
	//The + 1 is because at the beginning of the string section, we write
	//the number of string literals
	// * 3: as debug data uses 2 QWORDs per variable and a variable is 1 QWORD
	ChunkReserve(chunk, table->str_table.all_str_size + (table->str_table.count + 1) * sizeof(QWORD) + table->glob_table.count * 3 * sizeof(QWORD));

	//write GLOBAL offset
	ChunkWriteGLOBALSection(chunk, gen_global_pool(chunk, &table->glob_table));
	//write CONST offset
	ChunkWriteCONSTSection(chunk, gen_const_pool(chunk, &table->glob_table));
	//write string literals in the constant pool
	ChunkWriteSTRINGSection(chunk, gen_string_literal_pool(chunk, &table->str_table));

	//write DEBUG offset
	ChunkWriteDEBUGSection(chunk, gen_debug_pool(chunk, table));
	//write CODE offset
	ChunkWriteCODESection(chunk, chunk->count);

	bool is_valid = true;
	for (size_t i = 0; i < array->count - 1 && is_valid; i++)
	{
		is_valid = gen_byte_code(chunk, table, array->expressions[i]);
		//As we only implemented expressions, after an expression, we pop the result
		if (array->expressions[i]->expr_type.type_id != ID_COLT_VOID)
			ChunkWriteOpCode(chunk, OP_POP);
	}
	is_valid = gen_byte_code(chunk, table, array->expressions[array->count - 1]);
	if (is_valid)
	{		
		if (print_last_expr)
		{
			//Print the last expression
			ChunkWriteOpCode(chunk, OP_PRINT);
			ChunkWriteOperand(chunk, (BuiltinTypeID)array->expressions[array->count - 1]->expr_type.type_id);
		}
		//Pop the last expression
		ChunkWriteOpCode(chunk, OP_POP);
		//EXIT with code 0
		ChunkWriteOpCode(chunk, OP_EXIT);
		QWORD exit_code = { .i64 = 0 };
		ChunkWriteQWORD(chunk, exit_code);		
		return true;
	}
	return false;
}

uint64_t gen_global_pool(Chunk* chunk, const GlobalTable* glob_table)
{
	if (glob_table->count == 0)
		return 0;
	const uint64_t global_begin = chunk->count;

	for (size_t i = 0; i < glob_table->capacity; i++)
	{
		//not active entry
		if (glob_table->entries[i].key.ptr == NULL)
			continue;
		//FIXME: when type contains const flag
		//if (!glob_table->entries[i].is_const) //we are generating non-const
		{
			*((QWORD*)(chunk->code + global_begin) + glob_table->entries[i].counter_nb) = glob_table->entries[i].value;
			chunk->count += sizeof(QWORD);
		}
	}
	return global_begin;
}

uint64_t gen_const_pool(Chunk* chunk, const GlobalTable* glob_table)
{
	if (glob_table->count - glob_table->global_counter == 0)
		return 0;
	const uint64_t const_begin = chunk->count;

	for (size_t i = 0; i < glob_table->capacity; i++)
	{
		//not active entry
		if (glob_table->entries[i].key.ptr == NULL)
			continue;
		//FIXME: when type contains const flag
		//if (!glob_table->entries[i].is_const) //we are generating const
		if (false)
		{
			*((QWORD*)(chunk->code + const_begin) + glob_table->entries[i].counter_nb) = glob_table->entries[i].value;
			chunk->count += sizeof(QWORD);
		}
	}
	return const_begin;
}

uint64_t gen_string_literal_pool(Chunk* chunk, const StringTable* str_table)
{
	if (str_table->count == 0)
		return 0;

	const uint64_t string_begin = chunk->count;
	//Write the literal count first
	*((uint64_t*)(chunk->code + string_begin)) = str_table->count;

	uint64_t string_literal_begin = string_begin + (str_table->count + 1) * sizeof(QWORD);
	for (size_t i = 0; i < str_table->capacity; i++)
	{
		//not active entry
		if (str_table->str_entries[i].key.ptr == NULL)
			continue;
		//Write the byte offset to the beginning of the literal string
		//As each string contains a 'counter_nb', which goes from 0 -> count
		//we use that counter as the offset to add to the beginning of the string section
		//to get to the byte offset to the beginning of the literal string.
		// + 1 as the string section begins with the string literal count.
		*((uint64_t*)(chunk->code + string_begin) + str_table->str_entries[i].counter_nb + 1)
			= string_literal_begin;

		//Copy the string literal at the end of the byte offset section of the STRING section
		//This can be done as we reserved enough memory before having this function called
		memcpy(chunk->code + string_literal_begin, str_table->str_entries[i].key.ptr, 
			str_table->str_entries[i].key.size);
		string_literal_begin += str_table->str_entries[i].key.size;
	}
	//Update size of chunk
	chunk->count += str_table->all_str_size + (str_table->count + 1) * sizeof(QWORD);
	
	//Pad so that the next section does not have to worry about alignment
	uint64_t padding = 8 - (chunk->count % 8);
	for (size_t i = 0; i < padding; i++)
		chunk_write_byte(chunk, 205); //CD in hex
	return string_begin;
}

uint64_t gen_debug_pool(Chunk* chunk, const ASTTable* table)
{
	if (table->glob_table.count == 0)
		return 0;
	const uint64_t debug_begin = chunk->count;

	uint64_t string_literal_begin = debug_begin + table->glob_table.count * 2 * sizeof(QWORD);
	//Update size of chunk
	chunk->count += table->glob_table.count * 2 * sizeof(QWORD);
	for (size_t i = 0; i < table->glob_table.capacity; i++)
	{
		//not active entry
		if (table->glob_table.entries[i].key.ptr == NULL)
			continue;
		*((uint64_t*)(chunk->code + debug_begin) + table->glob_table.entries[i].counter_nb * 2)
			= table->glob_table.entries[i].type.type_id;
		*((uint64_t*)(chunk->code + debug_begin) + table->glob_table.entries[i].counter_nb * 2 + 1)
			= string_literal_begin;

		//We cannot memcpy, as we did not reserve memory for the strings in the DEBUG section
		for (size_t j = 0; j < table->glob_table.entries[i].key.size; j++)
			chunk_write_byte(chunk, table->glob_table.entries[i].key.ptr[j]);
		string_literal_begin += table->glob_table.entries[i].key.size;
	}	
	
	return debug_begin;
}

/*************************************
IMPLEMENTATION HELPERS
*************************************/

bool gen_byte_code(Chunk* chunk, const ASTTable* table, const Expr* expr)
{
	colt_assert(expr != NULL, "Generation should never happen if AST was not valid!");
	switch (expr->identifier)
	{
	case EXPR_UNARY:
		return gen_code_unary(chunk, table, (const UnaryExpr*)expr);
	case EXPR_BINARY:
		return gen_code_binary(chunk, table, (const BinaryExpr*)expr);
	case EXPR_LITERAL:
		return gen_code_literal(chunk, table, (const LiteralExpr*)expr);
	case EXPR_CONVERT:
		return gen_code_convert(chunk, table, (const ConvertExpr*)expr);
	case EXPR_GLOB_READ:
		return gen_global_read(chunk, table, (const GlobalReadExpr*)expr);
	case EXPR_GLOB_WRITE:
		return gen_global_write(chunk, table, (const GlobalWriteExpr*)expr);
	default:
		colt_assert(false, "NOT IMPLEMENTED YET!");
	}
	return true;
}

bool gen_code_unary(Chunk* chunk, const ASTTable* table, const UnaryExpr* ptr)
{
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_MINUS:
		gen_byte_code(chunk, table, ptr->child);
		ChunkWriteOpCode(chunk, OP_NEGATE);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->child->expr_type.type_id);
	break; case TKN_OPERATOR_PLUS:
		//DOES NOT DO ANYTHING
	break; case TKN_OPERATOR_BANG:
		gen_byte_code(chunk, table, ptr->child);
		ChunkWriteOpCode(chunk, OP_BOOL_NOT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->child->expr_type.type_id);
	break; case TKN_OPERATOR_TILDE:
		gen_byte_code(chunk, table, ptr->child);
		ChunkWriteOpCode(chunk, OP_BIT_NOT);
		ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);
	break; default:
		colt_assert(false, "Operator was not unary!");
		return false;
	}
	return true;
}

bool gen_code_binary(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr)
{
	switch (ptr->expr_operator)
	{	
	case TKN_OPERATOR_AND_AND:
		return gen_and_and_bool_comparison(chunk, table, ptr);

	case TKN_OPERATOR_OR_OR:
		return gen_or_or_bool_comparison(chunk, table, ptr);
	}

	gen_byte_code(chunk, table, ptr->lhs);
	gen_byte_code(chunk, table, ptr->rhs);

	colt_assert(ptr->expr_type.type_id <= ID_COLT_DOUBLE, "Type ID should be of that of a built-in type!");
	switch (ptr->expr_operator)
	{
	case TKN_OPERATOR_PLUS:
		gen_binary_plus();
		return true;
	case TKN_OPERATOR_MINUS:
		gen_binary_minus();
		return true;
	case TKN_OPERATOR_STAR:
		gen_binary_star();
		return true;
	case TKN_OPERATOR_SLASH:
		gen_binary_slash();
		return true;
	case TKN_OPERATOR_MODULO:
		gen_binary_modulo();
		return true;

		/****************************************
		* BITWISE OPCODES
		****************************************/

	case TKN_OPERATOR_AND:
		gen_binary_and();
		return true;
	case TKN_OPERATOR_OR:
		gen_binary_or();
		return true;
	case TKN_OPERATOR_XOR:
		gen_binary_xor();
		return true;

	case TKN_OPERATOR_GREATER_GREATER:
		gen_binary_rshift();
		return true;
	case TKN_OPERATOR_LESS_LESS:
		gen_binary_lshift();
		return true;
	
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

bool gen_code_literal(Chunk* chunk, const ASTTable* table, const LiteralExpr* ptr)
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
	case ID_COLT_LSTRING:
	{
		ChunkWriteOpCode(chunk, OP_PUSH_QWORD);
		const StringEntry* entry = string_table_find_entry(table->str_table.str_entries, table->str_table.capacity,
			StringToStringView(ptr->value.string_ptr));
		colt_assert(entry != NULL, "Could not find string literal entry!");
		//We write the string number
		QWORD offset = { .u64 = entry->counter_nb };
		ChunkWriteQWORD(chunk, offset);
		//Then load it (the interpreter converts it to a const char*)
		ChunkWriteOpCode(chunk, OP_LOAD_LSTRING);
	}
	break; default:
		colt_assert(false, "Type ID should be of that of a built-in type!");
		return false;
	}
	return true;
}

bool gen_code_convert(Chunk* chunk, const ASTTable* table, const ConvertExpr* ptr)
{
	gen_byte_code(chunk, table, ptr->child);
	ChunkWriteOpCode(chunk, OP_CONVERT);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->child->expr_type.type_id);
	ChunkWriteOperand(chunk, (BuiltinTypeID)ptr->expr_type.type_id);	
	return true;
}

bool gen_global_read(Chunk* chunk, const ASTTable* table, const GlobalReadExpr* ptr)
{
	const GlobalEntry* entry = variable_table_find_entry(table->glob_table.entries, table->glob_table.capacity, ptr->var_name);	
	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	//byte-offset to QWORD to load
	QWORD offset = { .u64 = entry->counter_nb * sizeof(QWORD) + ChunkGetGLOBALSection(chunk) };
	ChunkWriteOpCode(chunk, OP_LOAD_GLOBAL);
	ChunkWriteQWORD(chunk, offset);
	
	if (ptr->expr_type.type_id == ID_COLT_LSTRING)
		ChunkWriteOpCode(chunk, OP_LOAD_LSTRING);

	return true;
}

bool gen_global_write(Chunk* chunk, const ASTTable* table, const GlobalWriteExpr* ptr)
{
	//generate byte-code which pushes the value to write to the global
	gen_byte_code(chunk, table, ptr->value);

	//Optimize lstring assignment:
	//as every lstring loading writes a OP_LOAD_LSTRING,
	//there is no point in OP_STORE_LSTRING, we rather pop OP_LOAD_LSTRING
	if (ptr->value->expr_type.type_id == ID_COLT_LSTRING)
		chunk->count--;

	const GlobalEntry* entry = variable_table_find_entry(table->glob_table.entries, table->glob_table.capacity, ptr->var_name);
	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	//byte-offset from where to read
	QWORD offset = { .u64 = entry->counter_nb * sizeof(QWORD) + ChunkGetGLOBALSection(chunk) };
	ChunkWriteOpCode(chunk, OP_STORE_GLOBAL);
	ChunkWriteQWORD(chunk, offset);
	if (ptr->expr_type.type_id == ID_COLT_LSTRING)
		ChunkWriteOpCode(chunk, OP_LOAD_LSTRING);
	return true;
}

bool gen_and_and_bool_comparison(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr)
{
	colt_assert(ptr->expr_type.type_id == ID_COLT_BOOL, "Operands of && should be of type bool!");
	gen_byte_code(chunk, table, ptr->lhs);
	
	//If the condition is false, we do not evaluate the second, by jumping over it
	ChunkWriteOpCode(chunk, OP_SJUMP_NOT_TRUE);
	uint8_t* jump_to = chunk->code + chunk->count;
	//TO BE OVERRIDEN
	chunk_write_byte(chunk, 205);
	
	//If it is true, we pop the true, to evaluate the second condition
	ChunkWriteOpCode(chunk, OP_POP);
	gen_byte_code(chunk, table, ptr->rhs);
	
	//MIGHT OVERFLOW
	*jump_to = (uint8_t)((chunk->code + chunk->count) - jump_to);
	
	return true;
}

bool gen_or_or_bool_comparison(Chunk* chunk, const ASTTable* table, const BinaryExpr* ptr)
{
	colt_assert(ptr->expr_type.type_id == ID_COLT_BOOL, "Operands of || should be of type bool!");
	gen_byte_code(chunk, table, ptr->lhs);
	
	//If the condition is true, we do not evaluate the second, by jumping over it
	ChunkWriteOpCode(chunk, OP_SJUMP_TRUE);
	uint8_t* jump_to = chunk->code + chunk->count;
	//TO BE OVERRIDEN
	chunk_write_byte(chunk, 205);
	
	//If it is false, we pop the false, to evaluate the second condition
	ChunkWriteOpCode(chunk, OP_POP);
	gen_byte_code(chunk, table, ptr->rhs);
	
	//MIGHT OVERFLOW
	*jump_to = (uint8_t)((chunk->code + chunk->count) - jump_to);

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
	bit_size.u64 = type.byte_size * 8;
	//Cannot shift by a negative number
	gen_integral_short_jmp(chunk, OP_SJUMP_GREATER_EQ, zero, (BuiltinTypeID)type.type_id);
	//Cannot shift by a number greater than the bit size of the left hand side
	gen_integral_short_jmp(chunk, OP_SJUMP_LESS, bit_size, (BuiltinTypeID)type.type_id);
}

void gen_signed_addition_checks(Chunk* chunk, BuiltinTypeID type)
{
	//TODO: implement
}

void gen_signed_subtraction_checks(Chunk* chunk, BuiltinTypeID type)
{
	//TODO: implement
}

void gen_signed_multiplication_checks(Chunk* chunk, BuiltinTypeID type)
{
	//TODO: implement
}

void gen_signed_division_checks(Chunk* chunk, BuiltinTypeID type)
{
	//TODO: implement
}

#undef gen_binary_lshift
#undef gen_binary_rshift
#undef gen_binary_xor
#undef gen_binary_or
#undef gen_binary_and
#undef gen_binary_modulo
#undef gen_binary_slash
#undef gen_binary_star
#undef gen_binary_minus
#undef gen_binary_plus