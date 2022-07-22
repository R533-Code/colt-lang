/** @file byte_code_generator.c
* Contains the definitions of the functions in 'byte_code_generator.h'
*/
#include "byte_code_generator.h"

/// @brief Generates the byte-code necessary for an addition, and its checks
#define gen_binary_plus()	do { \
							ChunkWriteOpCode(gen->chunk, OP_ADD); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a subtraction, and its checks
#define gen_binary_minus() do { \
							ChunkWriteOpCode(gen->chunk, OP_SUBTRACT); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a multiplication, and its checks
#define gen_binary_star() do { \
							ChunkWriteOpCode(gen->chunk, OP_MULTIPLY); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a division, and its checks
#define gen_binary_slash() do { \
								ChunkWriteOpCode(gen->chunk, OP_DIVIDE); \
								ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a modulo operation, and its checks
#define gen_binary_modulo() do { \
								ChunkWriteOpCode(gen->chunk, OP_MODULO); \
								ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a bitwise AND, and its checks
#define gen_binary_and() do { colt_assert(is_type_integral(ptr->expr_type), "Type should be integral!"); \
							ChunkWriteOpCode(gen->chunk, OP_BIT_AND); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a bitwise OR, and its checks
#define gen_binary_or() do { colt_assert(is_type_integral(ptr->expr_type), "Type should be integral!"); \
							ChunkWriteOpCode(gen->chunk, OP_BIT_OR); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a bitwise XOR, and its checks
#define gen_binary_xor() do { colt_assert(is_type_integral(ptr->expr_type), "Type should be integral!"); \
							ChunkWriteOpCode(gen->chunk, OP_BIT_XOR); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a bitwise left shift, and its checks
#define gen_binary_lshift() do { \
							ChunkWriteOpCode(gen->chunk, OP_BIT_SHIFT_L); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)

/// @brief Generates the byte-code necessary for a bitwise right shift, and its checks
#define gen_binary_rshift() do { \
							ChunkWriteOpCode(gen->chunk, OP_BIT_SHIFT_R); \
							ChunkWriteOperand(gen->chunk, (BuiltinTypeID)TypeGetID(ptr->expr_type)); } while (0)


Chunk generateByteCode(const ASTTable* table, const ExprArray* array)
{
	colt_assert(array->count != 0, "Cannot generate byte-code if AST reported an error!");

	Chunk chunk;
	ChunkInit(&chunk);
	//Reserve enough size for the GLOBAL/CONST and string literals
	//The + 1 is because at the beginning of the string section, we write
	//the number of string literals
	// * 3: as debug data uses 2 QWORDs per variable and a variable is 1 QWORD
	ChunkReserve(&chunk, table->str_table.all_str_size + (table->str_table.count + 1) * sizeof(QWORD) + table->glob_table.count * 3 * sizeof(QWORD));

	//write GLOBAL and CONST offset
	gen_global_pool(&chunk, &table->glob_table);
	//write string literals in the constant pool
	gen_string_literal_pool(&chunk, &table->str_table);
	//write DEBUG offset
	gen_debug_pool(&chunk, table);

	//write CODE offset
	ChunkWriteCODESection(&chunk, chunk.count);

	ByteCodeGenerator gen = { .chunk = &chunk, .table = table};
	for (size_t i = 0; i < array->count - 1; i++)
	{
		gen_byte_code(array->expressions[i], &gen);
		if (TypeGetID(array->expressions[i]->expr_type) != ID_COLT_VOID)
			ChunkWriteOpCode(&chunk, OP_POP);
	}
	//Generate last expression
	//TODO: flag for printing last expression
	gen_byte_code(expr_array_back(array), &gen);
	if (ExprGetID(expr_array_back(array)) != ID_COLT_VOID)
		ChunkWriteOpCode(&chunk, OP_POP);
	
	ChunkWriteOpCode(&chunk, OP_EXIT);
	//Exit successfully
	ChunkWriteU64(&chunk, 0);
	
	return chunk;
}

/*************************************
IMPLEMENTATION HELPERS
*************************************/

void gen_byte_code(const Expr* expr, ByteCodeGenerator* gen)
{
	colt_assert(expr != NULL, "Generation should never happen if AST was not valid!");
	switch (expr->identifier)
	{
	break; case EXPR_UNARY:
		gen_code_unary((const UnaryExpr*)expr, gen);
	break; case EXPR_BINARY:
		gen_code_binary((const BinaryExpr*)expr, gen);
	break; case EXPR_LITERAL:
		gen_code_literal((const LiteralExpr*)expr, gen);
	break; case EXPR_CONVERT:
		gen_code_convert((const ConvertExpr*)expr, gen);
	break; case EXPR_CONDITION:
		gen_code_condition((const ConditionExpr*)expr, gen);
	break; case EXPR_SCOPE:
		gen_code_scope((const ScopeExpr*)expr, gen);
	break; case EXPR_WHILE:
		gen_code_while((const WhileExpr*)expr, gen);
	break; case EXPR_LOCAL_READ:
		gen_local_read((const LocalReadExpr*)expr, gen);
	break; case EXPR_LOCAL_WRITE:
		gen_local_write((const LocalWriteExpr*)expr, gen);
	break; case EXPR_GLOB_READ:
		gen_global_read((const GlobalReadExpr*)expr, gen);
	break; case EXPR_GLOB_WRITE:
		gen_global_write((const GlobalWriteExpr*)expr, gen);
	break; default:
		colt_unreachable("NOT IMPLEMENTED YET!");
	}
}

void gen_code_unary(const UnaryExpr* ptr, ByteCodeGenerator* gen)
{
	gen_byte_code(ptr->child, gen);
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_MINUS:
		ChunkWriteOpCode(gen->chunk, OP_NEGATE);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->child));
	break; case TKN_OPERATOR_PLUS:
		//DOES NOT DO ANYTHING
	break; case TKN_OPERATOR_BANG:
		ChunkWriteOpCode(gen->chunk, OP_BOOL_NOT);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->child));
	break; case TKN_KEYWORD_STATIC_PRINT:
		ChunkWriteOpCode(gen->chunk, OP_PRINT);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->child));
		ChunkWriteOpCode(gen->chunk, OP_POP);
	break; case TKN_OPERATOR_TILDE:
		ChunkWriteOpCode(gen->chunk, OP_BIT_NOT);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr));
	break; default:
		colt_unreachable("Operator was not unary!");
	}
}

void gen_code_binary(const BinaryExpr* ptr, ByteCodeGenerator* gen)
{
	switch (ptr->expr_operator)
	{	
	break; case TKN_OPERATOR_AND_AND:
		gen_and_and_bool_comparison(ptr, gen);

	break; case TKN_OPERATOR_OR_OR:
		gen_or_or_bool_comparison(ptr, gen);

	break; default:
		break;
	}

	gen_byte_code(ptr->lhs, gen);
	gen_byte_code(ptr->rhs, gen);

	colt_assert(TypeGetID(ptr->expr_type) <= ID_COLT_DOUBLE, "Type ID should be of that of a built-in type!");
	switch (ptr->expr_operator)
	{
	break; case TKN_OPERATOR_PLUS:
		gen_binary_plus();
	break; case TKN_OPERATOR_MINUS:
		gen_binary_minus();
	break; case TKN_OPERATOR_STAR:
		gen_binary_star();
	break; case TKN_OPERATOR_SLASH:
		gen_binary_slash();
	break; case TKN_OPERATOR_MODULO:
		gen_binary_modulo();

		/****************************************
		* BITWISE OPCODES
		****************************************/

	break; case TKN_OPERATOR_AND:
		gen_binary_and();
	break; case TKN_OPERATOR_OR:
		gen_binary_or();
	break; case TKN_OPERATOR_XOR:
		gen_binary_xor();
	break; case TKN_OPERATOR_GREATER_GREATER:
		gen_binary_rshift();
	break; case TKN_OPERATOR_LESS_LESS:
		gen_binary_lshift();
	
		/****************************************
		* COMPARISON OPCODES
		****************************************/

		//We write the left hand side's ID as byte-code comparisons expects the type of the QWORDs to compare
	break; case TKN_OPERATOR_GREATER:
		ChunkWriteOpCode(gen->chunk, OP_CMP_GREATER);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));
	break; case TKN_OPERATOR_GREATER_EQUAL:
		ChunkWriteOpCode(gen->chunk, OP_CMP_GREATER_EQ);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));
	break; case TKN_OPERATOR_LESS:
		ChunkWriteOpCode(gen->chunk, OP_CMP_LESS);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));
	break; case TKN_OPERATOR_LESS_EQUAL:
		ChunkWriteOpCode(gen->chunk, OP_CMP_LESS_EQ);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));
	break; case TKN_OPERATOR_EQUAL_EQUAL:
		ChunkWriteOpCode(gen->chunk, OP_CMP_EQUAL);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));
	break; case TKN_OPERATOR_BANG_EQUAL:
		ChunkWriteOpCode(gen->chunk, OP_CMP_NOT_EQUAL);
		ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->lhs));

	break; default:
		colt_unreachable("NOT IMPLEMENTED!");
	}
}

void gen_code_literal(const LiteralExpr* ptr, ByteCodeGenerator* gen)
{
	switch (TypeGetID(ptr->expr_type))
	{
	case ID_COLT_I8:
	case ID_COLT_U8:
	case ID_COLT_BOOL:
	case ID_COLT_CHAR:
		ChunkWriteOpCode(gen->chunk, OP_PUSH_BYTE);
		ChunkWriteBYTE(gen->chunk, ptr->value.byte);
		break;
	case ID_COLT_DOUBLE:
	case ID_COLT_U64:
	case ID_COLT_I64:
		ChunkWriteOpCode(gen->chunk, OP_PUSH_QWORD);
		ChunkWriteQWORD(gen->chunk, ptr->value);
		break;
	case ID_COLT_FLOAT:
	case ID_COLT_I32:
	case ID_COLT_U32:
		ChunkWriteOpCode(gen->chunk, OP_PUSH_DWORD);
		ChunkWriteDWORD(gen->chunk, ptr->value.dword);
		break;
	case ID_COLT_I16:
	case ID_COLT_U16:
		ChunkWriteOpCode(gen->chunk, OP_PUSH_WORD);
		ChunkWriteWORD(gen->chunk, ptr->value.word);
		break;
	case ID_COLT_LSTRING:
	{
		ChunkWriteOpCode(gen->chunk, OP_PUSH_QWORD);
		const StringEntry* entry = string_table_find_entry(gen->table->str_table.str_entries, gen->table->str_table.capacity,
			StringToStringView(ptr->value.string_ptr));
		colt_assert(entry != NULL, "Could not find string literal entry!");
		
		QWORD offset = { .u64 = 0 };
		for (size_t i = 0; i < gen->table->str_table.count; i++)
		{
			if (entry == gen->table->str_table.insertion_order[i])
			{
				offset.u64 = i;
				break;
			}
		}
		//We write the string number
		ChunkWriteQWORD(gen->chunk, offset);
		//Then load it (the interpreter converts it to a const char*)
		ChunkWriteOpCode(gen->chunk, OP_LOAD_LSTRING);
	}
	break; default:
		colt_unreachable("Type ID should be of that of a built-in type!");
	}
}

void gen_code_convert(const ConvertExpr* ptr, ByteCodeGenerator* gen)
{
	gen_byte_code(ptr->child, gen);
	ChunkWriteOpCode(gen->chunk, OP_CONVERT);
	ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr->child));
	ChunkWriteOperand(gen->chunk, (BuiltinTypeID)ExprGetID(ptr));
}

void gen_code_condition(const ConditionExpr* ptr, ByteCodeGenerator* gen)
{
	colt_assert(ptr->elif_conditions.count == ptr->elif_executes.count, "elif conditions count should match elif executes count!");

	gen_byte_code(ptr->if_condition, gen);
	ChunkWriteOpCode(gen->chunk, OP_JUMP_NOT_TRUE);
	
	DWORD uninitialized_offset = { .i32 = 0xffffffff };
	
	uint64_t to_override_jmp_next = gen->chunk->count;
	to_override_jmp_next += ChunkWriteDWORD(gen->chunk, uninitialized_offset);

	gen_byte_code(ptr->if_execute, gen);

	//byte offset to the uint32_t to override
	uint64_t to_override_jmp_out = 0;
	if (ptr->elif_conditions.count != 0 || ptr->else_execute != NULL)
	{
		//Write the jump to 'exit' of the if execute expression
		ChunkWriteOpCode(gen->chunk, OP_JUMP);
		to_override_jmp_out = gen->chunk->count;
		to_override_jmp_out += ChunkWriteDWORD(gen->chunk, uninitialized_offset);
	}
	//Override 0xffffffff
	*((uint32_t*)(gen->chunk->code + to_override_jmp_next)) = (uint32_t)gen->chunk->count;

	//Heap allocated array of DWORD to override for elif jumps
	uint64_t* to_override_jmp_out_array = NULL;

	if (ptr->elif_conditions.count != 0)
		to_override_jmp_out_array = safe_malloc(ptr->elif_conditions.count * sizeof(uint64_t));

	for (size_t i = 0; i < ptr->elif_conditions.count; i++)
	{
		gen_byte_code(ptr->elif_conditions.expressions[i], gen);
		ChunkWriteOpCode(gen->chunk, OP_JUMP_NOT_TRUE);
		//Store the jump to the next condition to test
		to_override_jmp_next = gen->chunk->count;
		to_override_jmp_next += ChunkWriteDWORD(gen->chunk, uninitialized_offset);

		gen_byte_code(ptr->elif_executes.expressions[i], gen);
		
		//Write the jump to 'exit' of the elif execute expression
		ChunkWriteOpCode(gen->chunk, OP_JUMP);
		to_override_jmp_out_array[i] = gen->chunk->count;
		to_override_jmp_out_array[i] += ChunkWriteDWORD(gen->chunk, uninitialized_offset);
		
		//Write the jump offset, which is after the 'exit' jump
		*((uint32_t*)(gen->chunk->code + to_override_jmp_next)) = (uint32_t)gen->chunk->count;
	}
	
	if (ptr->else_execute != NULL)
		gen_byte_code(ptr->else_execute, gen);
	
	//If the interior of the if is executed, we need to jump to after all the elif, and else byte-code
	if (to_override_jmp_out != 0)
		*((uint32_t*)(gen->chunk->code + to_override_jmp_out)) = (uint32_t)gen->chunk->count;
	if (to_override_jmp_out_array != NULL)
	{
		for (size_t i = 0; i < ptr->elif_conditions.count; i++)
			*((uint32_t*)(gen->chunk->code + to_override_jmp_out_array[i])) = (uint32_t)gen->chunk->count;
		safe_free(to_override_jmp_out_array);
	}
}

void gen_local_read(const LocalReadExpr* ptr, ByteCodeGenerator* gen)
{
	colt_assert(ptr->offset < 256, "Offset to cannot be represented");

	ChunkWriteOpCode(gen->chunk, OP_SLOAD_LOCAL);
	BYTE byte = { .u8 = (uint8_t)ptr->offset };
	ChunkWriteBYTE(gen->chunk, byte);
}

void gen_local_write(const LocalWriteExpr* ptr, ByteCodeGenerator* gen)
{
	colt_assert(ptr->offset < 256, "Offset to cannot be represented");
	
	gen_byte_code(ptr->value, gen);

	ChunkWriteOpCode(gen->chunk, OP_SSTORE_LOCAL);
	BYTE byte = { .u8 = (uint8_t)ptr->offset };
	ChunkWriteBYTE(gen->chunk, byte);
}

void gen_code_while(const WhileExpr* ptr, ByteCodeGenerator* gen)
{
	DWORD uninitialized_offset = { .i32 = 0xffffffff };

	uint64_t cond_begin = gen->chunk->count;
	
	gen_byte_code(ptr->while_condition, gen);
	ChunkWriteOpCode(gen->chunk, OP_JUMP_NOT_TRUE);

	uint64_t jump_out = gen->chunk->count;
	jump_out += ChunkWriteDWORD(gen->chunk, uninitialized_offset);

	gen_byte_code(ptr->while_body, gen);
	ChunkWriteOpCode(gen->chunk, OP_JUMP);
	uint64_t jmp_cond_begin = gen->chunk->count;
	jmp_cond_begin += ChunkWriteDWORD(gen->chunk, uninitialized_offset);
	*((uint32_t*)(gen->chunk->code + jmp_cond_begin)) = (uint32_t)cond_begin;

	*((uint32_t*)(gen->chunk->code + jump_out)) = (uint32_t)gen->chunk->count;
}

void gen_code_scope(const ScopeExpr* ptr, ByteCodeGenerator* gen)
{
	ChunkWriteOpCode(gen->chunk, OP_PUSH_SCOPE);
	DWORD dword = { .u32 = (uint32_t)ptr->var_count };
	ChunkWriteDWORD(gen->chunk, dword);
	
	for (size_t i = 0; i < ptr->array.count; i++)
	{
		gen_byte_code(ptr->array.expressions[i], gen);
		if (!ExprTypeEqualTypeID(ptr->array.expressions[i],ID_COLT_VOID))
			ChunkWriteOpCode(gen->chunk, OP_POP);
	}

	ChunkWriteOpCode(gen->chunk, OP_POP_SCOPE);
	dword.u32 = (uint32_t)ptr->var_count;
	ChunkWriteDWORD(gen->chunk, dword);
}

void gen_global_read(const GlobalReadExpr* ptr, ByteCodeGenerator* gen)
{
	const GlobalEntry* entry = variable_table_find_entry(gen->table->glob_table.entries, gen->table->glob_table.capacity, ptr->var_name);
	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	//byte-offset to QWORD to load
	QWORD offset = { .u64 = entry->counter_nb * sizeof(QWORD) + ChunkGetGLOBALSection(gen->chunk)  };
	ChunkWriteOpCode(gen->chunk, OP_LOAD_GLOBAL);
	ChunkWriteQWORD(gen->chunk, offset);
	
	if (ExprTypeEqualTypeID(ptr, ID_COLT_LSTRING))
		ChunkWriteOpCode(gen->chunk, OP_LOAD_LSTRING);
}

void gen_global_write(const GlobalWriteExpr* ptr, ByteCodeGenerator* gen)
{
	//generate byte-code which pushes the value to write to the global
	gen_byte_code(ptr->value, gen);

	//Optimize lstring assignment:
	//as every lstring loading writes a OP_LOAD_LSTRING,
	//there is no point in OP_STORE_LSTRING, we rather pop OP_LOAD_LSTRING
	if (ExprTypeEqualTypeID(ptr, ID_COLT_LSTRING))
		gen->chunk->count--;

	const GlobalEntry* entry = variable_table_find_entry(gen->table->glob_table.entries, gen->table->glob_table.capacity, ptr->var_name);
	colt_assert(entry->key.ptr != NULL, "Variable was not found!");

	//byte-offset from where to read
	QWORD offset = { .u64 = entry->counter_nb * sizeof(QWORD) + ChunkGetGLOBALSection(gen->chunk) };
	ChunkWriteOpCode(gen->chunk, OP_STORE_GLOBAL);
	ChunkWriteQWORD(gen->chunk, offset);
	
	if (ExprTypeEqualTypeID(ptr, ID_COLT_LSTRING))
		ChunkWriteOpCode(gen->chunk, OP_LOAD_LSTRING);
}

void gen_and_and_bool_comparison(const BinaryExpr* ptr, ByteCodeGenerator* gen)
{
	colt_assert(TypeGetID(ptr->expr_type) == ID_COLT_BOOL, "Operands of && should be of type bool!");
	gen_byte_code(ptr->lhs, gen);
	
	//If the condition is false, we do not evaluate the second, by jumping over it
	ChunkWriteOpCode(gen->chunk, OP_JUMP_NOT_TRUE);
	
	DWORD uninitialized_offset = { .u32 = 0xffffffff };
	uint32_t jump_to = (uint32_t)gen->chunk->count;
	jump_to += (uint32_t)ChunkWriteDWORD(gen->chunk, uninitialized_offset);
	
	//If it is true, we pop the true, to evaluate the second condition
	ChunkWriteOpCode(gen->chunk, OP_POP);
	gen_byte_code(ptr->rhs, gen);
	
	//Override jump offset
	*((uint32_t*)(gen->chunk->code + jump_to)) = (uint32_t)gen->chunk->count;
}

void gen_or_or_bool_comparison(const BinaryExpr* ptr, ByteCodeGenerator* gen)
{
	colt_assert(TypeGetID(ptr->expr_type) == ID_COLT_BOOL, "Operands of || should be of type bool!");
	gen_byte_code(ptr->lhs, gen);
	
	//If the condition is true, we do not evaluate the second, by jumping over it
	ChunkWriteOpCode(gen->chunk, OP_JUMP_TRUE);
	
	DWORD uninitialized_offset = { .u32 = 0xffffffff };
	uint32_t jump_to = (uint32_t)gen->chunk->count;
	jump_to += (uint32_t)ChunkWriteDWORD(gen->chunk, uninitialized_offset);
	
	//If it is false, we pop the false, to evaluate the second condition
	ChunkWriteOpCode(gen->chunk, OP_POP);
	gen_byte_code(ptr->rhs, gen);
	
	//Override jump offset
	*((uint32_t*)(gen->chunk->code + jump_to)) = (uint32_t)gen->chunk->count;
}

void gen_global_pool(Chunk* chunk, const GlobalTable* glob_table)
{
	if (glob_table->count == 0)
		return;
	const uint64_t global_begin = chunk->count;

	for (size_t i = 0; i < glob_table->capacity; i++)
	{
		//not active entry
		if (glob_table->entries[i].key.ptr == NULL)
			continue;

		*((QWORD*)(chunk->code + global_begin) + glob_table->entries[i].counter_nb) = glob_table->entries[i].value;
		chunk->count += sizeof(QWORD);
	}

	ChunkWriteGLOBALSection(chunk, global_begin);
}

void gen_string_literal_pool(Chunk* chunk, const StringTable* str_table)
{
	if (str_table->count == 0)
		return;

	const uint64_t string_begin = chunk->count;
	//Write the literal count first
	*((uint64_t*)(chunk->code + string_begin)) = str_table->count;

	uint64_t string_literal_begin = string_begin + (str_table->count + 1) * sizeof(QWORD);
	for (size_t i = 0; i < str_table->count; i++)
	{
		//Write the byte offset to the beginning of the literal string
		//As each string contains a 'counter_nb', which goes from 0 -> count
		//we use that counter as the offset to add to the beginning of the string section
		//to get to the byte offset to the beginning of the literal string.
		// + 1 as the string section begins with the string literal count.
		*((uint64_t*)(chunk->code + string_begin) + i + 1)
			= string_literal_begin;

		//Copy the string literal at the end of the byte offset section of the STRING section
		//This can be done as we reserved enough memory before having this function called
		memcpy(chunk->code + string_literal_begin, str_table->insertion_order[i]->key.ptr,
			str_table->insertion_order[i]->key.size);
		string_literal_begin += str_table->insertion_order[i]->key.size;
	}
	//Update size of chunk
	chunk->count += str_table->all_str_size + (str_table->count + 1) * sizeof(QWORD);

	//Pad so that the next section does not have to worry about alignment
	uint64_t padding = 8 - (chunk->count & 7); // % 8
	for (size_t i = 0; i < padding; i++)
		chunk_write_byte(chunk, 205); //CD in hex

	ChunkWriteSTRINGSection(chunk, string_begin);
}

void gen_debug_pool(Chunk* chunk, const ASTTable* table)
{
	if (table->glob_table.count == 0)
		return;
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
			= TypeGetID(table->glob_table.entries[i].type);
		*((uint64_t*)(chunk->code + debug_begin) + table->glob_table.entries[i].counter_nb * 2 + 1)
			= string_literal_begin;

		//We cannot memcpy, as we did not reserve memory for the strings in the DEBUG section
		for (size_t j = 0; j < table->glob_table.entries[i].key.size; j++)
			chunk_write_byte(chunk, table->glob_table.entries[i].key.ptr[j]);
		string_literal_begin += table->glob_table.entries[i].key.size;
	}

	ChunkWriteDEBUGSection(chunk, debug_begin);
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