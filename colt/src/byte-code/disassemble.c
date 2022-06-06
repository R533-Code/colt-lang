/** @file disassemble.c
* Contains the definitions of the functions declared in 'disassemble.h'
*/

#include "disassemble.h"

void ChunkDisassemble(const Chunk* chunk, const char* name)
{
	printf("============ %s ============\n", name);

	//32 is the size of the header
	if (chunk->count == 32)
	{
		printf("!EMPTY CHUNK!");
		return;
	}

	//GLOBAL OFFSET
	uint64_t global_offset = *((uint64_t*)chunk->code);
	//CONST OFFSET
	uint64_t const_offset = *((uint64_t*)chunk->code + 1);
	//DEBUG OFFSET
	uint64_t debug_offset = *((uint64_t*)chunk->code + 2);
	//CODE OFFSET
	uint64_t code_offset = *((uint64_t*)chunk->code + 3);

	fputs(CONSOLE_COLOR_REVERSE"SECTION HEADER:\n"CONSOLE_COLOR_RESET, stdout);
	printf("        %08"PRIu64": SECTION GLOBAL\n""        %08"PRIu64": SECTION CONST\n""        %08"PRIu64": SECTION DEBUG\n""        %08"PRIu64": SECTION CODE\n",
		global_offset, const_offset, debug_offset, code_offset
	);

	if (global_offset != 0)
	{
		fputs(CONSOLE_COLOR_REVERSE"SECTION GLOBAL:\n"CONSOLE_COLOR_RESET, stdout);
		const uint64_t global_end = const_offset > 0 ? const_offset : (debug_offset > 0 ? debug_offset : (code_offset > 0 ? code_offset : chunk->count));
		//if there are debug symbols
		if (debug_offset != 0)
		{
			for (size_t i = 0; i < (global_end - global_offset) / sizeof(QWORD); i++)
				impl_print_global_variable(chunk, global_offset, i, (BuiltinTypeID) *(chunk->code + debug_offset + i));
		}
		else
		{
			for (size_t i = global_offset; i < global_end; i += sizeof(QWORD))
				printf("        %08"PRIu64": 0x%"PRIX64, i, *(uint64_t*)chunk->code + i);
		}
	}
	if (const_offset != 0)
	{
		fputs(CONSOLE_COLOR_REVERSE"SECTION CONST:\n"CONSOLE_COLOR_RESET, stdout);
		const uint64_t const_end = debug_offset > 0 ? debug_offset : (code_offset > 0 ? code_offset : chunk->count);
		//if there are debug symbols
		if (debug_offset != 0)
		{
			for (size_t i = 0; i < const_end; i++)
				impl_print_global_variable(chunk, const_offset, i, (BuiltinTypeID) *(chunk->code + debug_offset + i));
		}
		else
		{
			for (size_t i = const_offset; i < const_end; i += sizeof(QWORD))
				printf("        %08"PRIu64": 0x%"PRIX64, i, *(uint64_t*)chunk->code + i);
		}
	}

	if (code_offset != 0)
	{
		//loop to print byte-code to execute
		fputs(CONSOLE_COLOR_REVERSE"SECTION CODE:\n"CONSOLE_COLOR_RESET, stdout);
		for (uint64_t offset = code_offset; offset < chunk->count;)
		{
			offset = impl_chunk_print_code(chunk, offset);
		}
	}

}

void impl_print_global_variable(const Chunk* chunk, uint64_t offset, uint64_t var_nb, BuiltinTypeID id)
{
	printf("        %08"PRIu64 CONSOLE_FOREGROUND_CYAN" '%s': "CONSOLE_FOREGROUND_BRIGHT_GREEN"'", offset + 8 * var_nb, BuiltinTypeIDToString(id));
		
	QWORD value = *((QWORD*)chunk->code + offset + var_nb * 8);
	OpCode_Print(value, id);
	fputs("'\n"CONSOLE_COLOR_RESET, stdout);
}

uint64_t impl_chunk_print_code(const Chunk* chunk, uint64_t offset)
{
	printf("        %08"PRIu64" ", offset);

	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
		/******************************************************/

	case OP_PUSH_BYTE:
		impl_print_hex_instruction("OP_PUSH_BYTE", ChunkGetBYTE(chunk, &offset).u8);
		return offset;

	case OP_PUSH_WORD:
		impl_print_hex_instruction("OP_PUSH_WORD", ChunkGetWORD(chunk, &offset).u16);
		return offset;

	case OP_PUSH_DWORD:
		impl_print_hex_instruction("OP_PUSH_DWORD", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

	case OP_PUSH_QWORD:
		impl_print_hex_instruction("OP_PUSH_QWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

	case OP_LOAD_BYTE:
		impl_print_hex_instruction("OP_LOAD_BYTE", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_LOAD_WORD:
		impl_print_hex_instruction("OP_LOAD_WORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_LOAD_DWORD:
		impl_print_hex_instruction("OP_LOAD_DWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_LOAD_QWORD:
		impl_print_hex_instruction("OP_LOAD_QWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

	case OP_STORE_BYTE:
		impl_print_hex_instruction("OP_STORE_BYTE", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_STORE_WORD:
		impl_print_hex_instruction("OP_STORE_WORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_STORE_DWORD:
		impl_print_hex_instruction("OP_STORE_DWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;
	case OP_STORE_QWORD:
		impl_print_hex_instruction("OP_STORE_QWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

		/******************************************************/

	case OP_CONVERT:
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
	case OP_POP:
		return impl_print_simple_instruction("OP_POP", offset);

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
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(type), offset + 2 + to_offset);
	return offset + 3;
}

void impl_print_hex_instruction(const char* name, uint64_t value)
{
	printf(CONSOLE_FOREGROUND_BRIGHT_BLUE "%-20s "
		CONSOLE_FOREGROUND_BRIGHT_GREEN "'0x%"PRIX64"'\n"
		CONSOLE_COLOR_RESET, name, value);
}