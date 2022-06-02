/** @file disassemble.c
* Contains the definitions of the functions declared in 'disassemble.h'
*/

#include "disassemble.h"

void ChunkDisassemble(const Chunk* chunk, const char* name)
{
	printf("============ %s ============\n", name);

	if (chunk->count == 0)
	{
		printf("!EMPTY CHUNK!");
		return;
	}
	for (uint64_t offset = 0; offset < chunk->count;)
	{
		offset = impl_chunk_print_code(chunk, offset);
	}
}

uint64_t impl_chunk_print_code(const Chunk* chunk, uint64_t offset)
{
	printf("%04"PRIu64" ", offset);

	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
		/******************************************************/

	case OP_IMMEDIATE_BYTE:
		colt_assert(offset + 1 <= chunk->count, "Missing byte after OP_IMMEDIATE_BYTE!");
		impl_print_hex_instruction("OP_IMMEDIATE_BYTE", ChunkGetBYTE(chunk, &offset).u8);
		return offset;

	case OP_IMMEDIATE_WORD:
		colt_assert(offset + ((uint64_t)(chunk->code + offset) & 1) + sizeof(int16_t) <= chunk->count, "Missing int16 after OP_IMMEDIATE_WORD");
		impl_print_hex_instruction("OP_IMMEDIATE_WORD", ChunkGetWORD(chunk, &offset).u16);
		return offset;

	case OP_IMMEDIATE_DWORD:
		colt_assert(offset + ((uint64_t)(chunk->code + offset) % 4) + sizeof(int32_t) <= chunk->count, "Missing int32 after OP_IMMEDIATE_DWORD");
		impl_print_hex_instruction("OP_IMMEDIATE_DWORD", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

	case OP_IMMEDIATE_QWORD:
		colt_assert(offset + ((uint64_t)(chunk->code + offset) % 8) + sizeof(int64_t) <= chunk->count, "Missing int64 after OP_IMMEDIATE_QWORD");
		impl_print_hex_instruction("OP_IMMEDIATE_QWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

		/******************************************************/

	case OP_CONVERT:
		colt_assert(offset + 2 <= chunk->count, "Missing operands of OP_CONVERT!");		
		return impl_print_2operand_instruction("OP_CONVERT", chunk->code[offset + 1], chunk->code[offset + 2], offset);

		/******************************************************/

	case OP_NEGATE:
		return impl_print_operand_instruction("OP_NEGATE", chunk->code[offset + 1], offset);

	case OP_ADD:
		return impl_print_operand_instruction("OP_ADD", chunk->code[offset + 1], offset);
	case OP_SUBTRACT:
		return impl_print_operand_instruction("OP_SUBTRACT", chunk->code[offset + 1], offset);
	case OP_MULTIPLY:
		return impl_print_operand_instruction("OP_MULTIPLY", chunk->code[offset + 1], offset);
	case OP_DIVIDE:
		return impl_print_operand_instruction("OP_DIVIDE", chunk->code[offset + 1], offset);
	case OP_BIT_AND:
		return impl_print_operand_instruction("OP_BIT_AND", chunk->code[offset + 1], offset);
	case OP_BIT_OR:
		return impl_print_operand_instruction("OP_BIT_OR", chunk->code[offset + 1], offset);
	case OP_BIT_XOR:
		return impl_print_operand_instruction("OP_BIT_XOR", chunk->code[offset + 1], offset);
	case OP_BIT_NOT:
		return impl_print_operand_instruction("OP_BIT_NOT", chunk->code[offset + 1], offset);
	case OP_BIT_SHIFT_L:
		return impl_print_operand_instruction("OP_BIT_SHIFT_L", chunk->code[offset + 1], offset);
	case OP_BIT_SHIFT_R:
		return impl_print_operand_instruction("OP_BIT_SHIFT_R", chunk->code[offset + 1], offset);
	case OP_CMP_GREATER:
		return impl_print_operand_instruction("OP_CMP_GREATER", chunk->code[offset + 1], offset);
	case OP_CMP_GREATER_EQ:
		return impl_print_operand_instruction("OP_CMP_GREATER_EQ", chunk->code[offset + 1], offset);
	case OP_CMP_LESS:
		return impl_print_operand_instruction("OP_CMP_LESS", chunk->code[offset + 1], offset);
	case OP_CMP_LESS_EQ:
		return impl_print_operand_instruction("OP_CMP_LESS_EQ", chunk->code[offset + 1], offset);
	case OP_CMP_EQUAL:
		return impl_print_operand_instruction("OP_CMP_EQUAL", chunk->code[offset + 1], offset);
	case OP_CMP_NOT_EQUAL:
		return impl_print_operand_instruction("OP_CMP_NOT_EQUAL", chunk->code[offset + 1], offset);

		/******************************************************/

	case OP_SJUMP_GREATER:
		return impl_print_sjump_instruction("OP_SJUMP_GREATER", chunk->code[offset + 1], chunk->code[offset + 2], offset);
	case OP_SJUMP_GREATER_EQ:
		return impl_print_sjump_instruction("OP_SJUMP_GREATER_EQ", chunk->code[offset + 1], chunk->code[offset + 2], offset);
	case OP_SJUMP_LESS:
		return impl_print_sjump_instruction("OP_SJUMP_LESS", chunk->code[offset + 1], chunk->code[offset + 2], offset);
	case OP_SJUMP_LESS_EQ:
		return impl_print_sjump_instruction("OP_SJUMP_LESS_EQ", chunk->code[offset + 1], chunk->code[offset + 2], offset);
	case OP_SJUMP_EQUAL:
		return impl_print_sjump_instruction("OP_SJUMP_EQUAL", chunk->code[offset + 1], chunk->code[offset + 2], offset);
	case OP_SJUMP_NOT_EQUAL:
		return impl_print_sjump_instruction("OP_SJUMP_NOT_EQUAL", chunk->code[offset + 1], chunk->code[offset + 2], offset);



		/******************************************************/

	case OP_PRINT:
		return impl_print_operand_instruction("OP_PRINT", chunk->code[offset + 1], offset);

		/******************************************************/

	case OP_RETURN:
		return impl_print_simple_instruction("OP_RETURN", offset);

	case OP_EXIT:
		impl_print_hex_instruction("OP_EXIT", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

	default:
		printf(CONSOLE_FOREGROUND_BRIGHT_RED "UNKOWN OPCODE: '%d'\n"
			CONSOLE_COLOR_RESET, instruction);
		return offset + 1;
	}
}

uint64_t impl_print_simple_instruction(const char* name, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_GREEN "%-20s\n"
		CONSOLE_COLOR_RESET, name);
	return offset + 1;
}

uint64_t impl_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_BRIGHT_MAGENTA "%-20s "
		CONSOLE_FOREGROUND_CYAN "'%s'\n"
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(byte));
	return offset + 2;
}

uint64_t impl_print_2operand_instruction(const char* name, uint8_t first, uint8_t second, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_YELLOW "%-20s "
		CONSOLE_FOREGROUND_CYAN "'%s'" CONSOLE_COLOR_RESET ", "
		CONSOLE_FOREGROUND_CYAN "'%s'\n"
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(first), BuiltinTypeIDToString(second));
	return offset + 3;
}

uint64_t impl_print_sjump_instruction(const char* name, uint8_t type, uint8_t to_offset, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_YELLOW "%-20s "
		CONSOLE_FOREGROUND_CYAN "'%s'" CONSOLE_COLOR_RESET ", "
		CONSOLE_FOREGROUND_CYAN "TO: '%"PRIu64"'\n"
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(type), offset + to_offset);
	return offset + 3;
}

void impl_print_hex_instruction(const char* name, uint64_t value)
{
	printf(CONSOLE_FOREGROUND_BRIGHT_BLUE "%-20s "
		CONSOLE_FOREGROUND_BRIGHT_GREEN "'0x%"PRIX64"'\n"
		CONSOLE_COLOR_RESET, name, value);
}