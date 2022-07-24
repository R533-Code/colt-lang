/** @file disassemble.c
* Contains the definitions of the functions declared in 'disassemble.h'
*/

#include "disassemble.h"

void ChunkDisassemble(const Chunk* chunk, const char* name)
{
	printf("================ %s ================\n", name);

	//size of the header
	if (chunk->count <= CHUNK_HEADER_QWORD_COUNT * sizeof(QWORD))
	{
		fputs("!EMPTY CHUNK!", stdout);
		return;
	}
	if (ChunkGetABI(chunk) != COLTI_ABI)
	{
		fputs(CONSOLE_FOREGROUND_BRIGHT_RED "Error: " CONSOLE_COLOR_RESET "Cannot disassemble Chunk as its ABI (", stderr);
		ChunkPrintABI(chunk, stderr);
		fputs(") does not match the VM's ABI (" COLTI_ABI_STRING ")!\n", stderr);
		exit(EXIT_USER_INVALID_INPUT);
	}

	//GLOBAL OFFSET
	uint64_t global_offset = ChunkGetGLOBALSection(chunk);
	//STRING OFFSET
	uint64_t string_offset = ChunkGetSTRINGSection(chunk);
	//DEBUG OFFSET
	uint64_t debug_offset = ChunkGetDEBUGSection(chunk);
	//CODE OFFSET
	uint64_t code_offset = ChunkGetCODESection(chunk);

	fputs(CONSOLE_COLOR_REVERSE"SECTION HEADER:\n"CONSOLE_COLOR_RESET, stdout);
	fputs("        ABI: ", stdout);
	ChunkPrintABI(chunk, stdout);
	
	if (memcmp(chunk->code + 8, ChunkSignature, 8) == 0)
		fputs(", found valid signature", stdout);
	//Debug informations are not? found
	printf("\n        DEBUG informations: %sfound", debug_offset != 0 ? "" : "not ");

	//Print informations about each section
	printf("\n        %08"PRIu64": SECTION GLOBAL\n        %08"PRIu64": SECTION STRING\n        %08"PRIu64": SECTION DEBUG\n        %08"PRIu64": SECTION CODE\n",
		global_offset, string_offset, debug_offset, code_offset
	);

	if (global_offset != 0)
	{
		fputs(CONSOLE_COLOR_REVERSE"SECTION GLOBAL:\n"CONSOLE_COLOR_RESET, stdout);
		const uint64_t global_end = unsafe_chunk_get_global_end(chunk);
		//if there are debug symbols
		if (debug_offset != 0)
		{
			for (size_t i = 0; i < (global_end - global_offset) / sizeof(QWORD); i++)
				unsafe_print_global_variable(chunk, i);
		}
		else
		{
			for (size_t i = global_offset; i < global_end; i += sizeof(QWORD))
				printf("        %08"PRIu64": 0x%"PRIX64"\n", i, *(uint64_t*)(chunk->code + global_offset + i));
		}
	}
	if (string_offset != 0)
	{
		fputs(CONSOLE_COLOR_REVERSE"SECTION STRING:\n"CONSOLE_COLOR_RESET, stdout);
		const uint64_t string_literal_count = unsafe_chunk_get_lstring_count(chunk);
		
		//%c for plural -> 's' else ' '
		printf("        %08"PRIu64" %"PRIu64" lstring%c\n",
			string_offset, string_literal_count, string_literal_count == 1 ? ' ' : 's');
		
		for (size_t i = 0; i < string_literal_count; i++)
		{
			printf("        %08"PRIu64": ", string_offset + (i + 1) * sizeof(QWORD));;	
			print_lstring(chunk->code + *((uint64_t*)(chunk->code + string_offset) + i + 1));
			fputc('\n', stdout);
		}
	}

	if (code_offset != 0)
	{
		fputs(CONSOLE_COLOR_REVERSE"SECTION CODE:\n"CONSOLE_COLOR_RESET, stdout);
		//loop to print byte-code to execute
		for (uint64_t offset = code_offset; offset < chunk->count;)
			offset = dis_chunk_print_code(chunk, offset);
	}

}

void print_lstring(const char* str)
{
	fputs(CONSOLE_FOREGROUND_YELLOW"\"", stdout);
	//TODO: we need to add size information for string
	while (*str != '\0')
	{
		switch (*str)
		{
		break; case '\'':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\'"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\"':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\\""CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\\':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\\\"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\a':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\a"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\b':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\b"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\f':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\f"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\n':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\n"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\r':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\r"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\t':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\t"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; case '\v':
			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW "\\v"CONSOLE_FOREGROUND_YELLOW, stdout);
		break; default:
			fputc(*str, stdout);
		}
		str++;
	}
	fputs("\""CONSOLE_COLOR_RESET, stdout);
}

void unsafe_print_global_variable(const Chunk* chunk, uint64_t var_nb)
{
	uint64_t debug_offset = ChunkGetDEBUGSection(chunk);
	colt_assert(debug_offset != 0, "Cannot be called if no debug data exist!");
	
	uint64_t offset = ChunkGetGLOBALSection(chunk);

	// + sizeof(QWORD) as we need to offset by one QWORD to get the offset to the name
	const char* name = chunk->code + *(uint64_t*)(chunk->code + debug_offset + var_nb * 2 * sizeof(QWORD) + sizeof(QWORD));
	BuiltinTypeID id = (BuiltinTypeID)((QWORD*)(chunk->code + debug_offset + var_nb * 2 * sizeof(QWORD)))->u8;
	printf("        %08"PRIu64 CONSOLE_FOREGROUND_CYAN" %s " CONSOLE_FOREGROUND_BRIGHT_CYAN "%s" CONSOLE_COLOR_RESET " = "CONSOLE_FOREGROUND_BRIGHT_GREEN, offset + 8 * var_nb, BuiltinTypeIDToString(id), name);
		
	QWORD value = *(QWORD*)(chunk->code + offset + var_nb * 8);
	if (id == ID_COLT_LSTRING)
		print_lstring(chunk->code + *(uint64_t*)(chunk->code + ChunkGetSTRINGSection(chunk) + (value.u64 + 1) * sizeof(QWORD)));
	else
		OpCode_Print(value, id);

	fputs(CONSOLE_COLOR_RESET ";\n", stdout);
}

uint64_t dis_chunk_print_code(const Chunk* chunk, uint64_t offset)
{
	printf("        %08"PRIu64" ", offset);

	uint8_t instruction = chunk->code[offset];
	switch (instruction)
	{
		/******************************************************/

	case OP_LOAD_GLOBAL:
		dis_print_global_instruction("OP_LOAD_GLOBAL", ChunkGetQWORD(chunk, &offset).u64, chunk);		
		return offset;

	case OP_STORE_GLOBAL:
		dis_print_global_instruction("OP_STORE_GLOBAL", ChunkGetQWORD(chunk, &offset).u64, chunk);
		return offset;

	case OP_LOAD_LSTRING:
		return dis_print_simple_instruction("OP_LOAD_LSTRING", offset);

	case OP_PUSH_BYTE:
		dis_print_hex_instruction("OP_PUSH_BYTE", ChunkGetBYTE(chunk, &offset).u8);
		return offset;

	case OP_PUSH_WORD:
		dis_print_hex_instruction("OP_PUSH_WORD", ChunkGetWORD(chunk, &offset).u16);
		return offset;

	case OP_PUSH_DWORD:
		dis_print_hex_instruction("OP_PUSH_DWORD", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

	case OP_PUSH_QWORD:
		dis_print_hex_instruction("OP_PUSH_QWORD", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

	case OP_PUSH_SCOPE:
		dis_print_hex_instruction("OP_PUSH_SCOPE", ChunkGetDWORD(chunk, &offset).u32);
		return offset;
	
	case OP_POP_SCOPE:
		dis_print_hex_instruction("OP_POP_SCOPE", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

	case OP_SLOAD_LOCAL:
		dis_print_hex_instruction("OP_SLOAD_LOCAL", ChunkGetBYTE(chunk, &offset).u8);
		return offset;

	case OP_SSTORE_LOCAL:
		dis_print_hex_instruction("OP_SSTORE_LOCAL", ChunkGetBYTE(chunk, &offset).u8);
		return offset;

	case OP_LOAD_LOCAL:
		dis_print_hex_instruction("OP_LOAD_LOCAL", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

	case OP_STORE_LOCAL:
		dis_print_hex_instruction("OP_STORE_LOCAL", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

		/******************************************************/

	case OP_CONVERT:
		return dis_print_2operand_instruction("OP_CONVERT", chunk->code[offset + 1], chunk->code[offset + 2], offset);

		/******************************************************/

	case OP_NEGATE:
		return dis_print_operand_instruction("OP_NEGATE", chunk->code[offset + 1], offset);

	case OP_ADD:
		return dis_print_operand_instruction("OP_ADD", chunk->code[offset + 1], offset);
	case OP_SUBTRACT:
		return dis_print_operand_instruction("OP_SUBTRACT", chunk->code[offset + 1], offset);
	case OP_MULTIPLY:
		return dis_print_operand_instruction("OP_MULTIPLY", chunk->code[offset + 1], offset);
	case OP_DIVIDE:
		return dis_print_operand_instruction("OP_DIVIDE", chunk->code[offset + 1], offset);
	case OP_MODULO:
		return dis_print_operand_instruction("OP_MODULO", chunk->code[offset + 1], offset);
	case OP_BIT_AND:
		return dis_print_operand_instruction("OP_BIT_AND", chunk->code[offset + 1], offset);
	case OP_BIT_OR:
		return dis_print_operand_instruction("OP_BIT_OR", chunk->code[offset + 1], offset);
	case OP_BIT_XOR:
		return dis_print_operand_instruction("OP_BIT_XOR", chunk->code[offset + 1], offset);
	case OP_BIT_NOT:
		return dis_print_operand_instruction("OP_BIT_NOT", chunk->code[offset + 1], offset);
	case OP_BIT_SHIFT_L:
		return dis_print_operand_instruction("OP_BIT_SHIFT_L", chunk->code[offset + 1], offset);
	case OP_BIT_SHIFT_R:
		return dis_print_operand_instruction("OP_BIT_SHIFT_R", chunk->code[offset + 1], offset);
	case OP_BOOL_NOT:
		return dis_print_operand_instruction("OP_BOOL_NOT", chunk->code[offset + 1], offset);
	case OP_CMP_GREATER:
		return dis_print_operand_instruction("OP_CMP_GREATER", chunk->code[offset + 1], offset);
	case OP_CMP_GREATER_EQ:
		return dis_print_operand_instruction("OP_CMP_GREATER_EQ", chunk->code[offset + 1], offset);
	case OP_CMP_LESS:
		return dis_print_operand_instruction("OP_CMP_LESS", chunk->code[offset + 1], offset);
	case OP_CMP_LESS_EQ:
		return dis_print_operand_instruction("OP_CMP_LESS_EQ", chunk->code[offset + 1], offset);
	case OP_CMP_EQUAL:
		return dis_print_operand_instruction("OP_CMP_EQUAL", chunk->code[offset + 1], offset);
	case OP_CMP_NOT_EQUAL:
		return dis_print_operand_instruction("OP_CMP_NOT_EQUAL", chunk->code[offset + 1], offset);

		/******************************************************/
	
	case OP_JUMP_FALSE_TPOP:
		dis_print_jump_instruction("OP_JUMP_FALSE_TPOP", ChunkGetDWORD(chunk, &offset).u32);
		return offset;
	case OP_JUMP_TRUE_FPOP:
		dis_print_jump_instruction("OP_JUMP_TRUE_FPOP", ChunkGetDWORD(chunk, &offset).u32);
		return offset;
	case OP_JUMP_FALSE:
		dis_print_jump_instruction("OP_JUMP_FALSE", ChunkGetDWORD(chunk, &offset).u32);
		return offset;
	case OP_JUMP_TRUE:
		dis_print_jump_instruction("OP_JUMP_TRUE", ChunkGetDWORD(chunk, &offset).u32);
		return offset;
	case OP_JUMP:
		dis_print_jump_instruction("OP_JUMP", ChunkGetDWORD(chunk, &offset).u32);
		return offset;

		/******************************************************/

	case OP_PRINT:
		return dis_print_operand_instruction("OP_PRINT", chunk->code[offset + 1], offset);

		/******************************************************/

	case OP_RETURN:
		return dis_print_simple_instruction("OP_RETURN", offset);
	case OP_POP:
		return dis_print_simple_instruction("OP_POP", offset);

	case OP_EXIT:
		dis_print_hex_instruction("OP_EXIT", ChunkGetQWORD(chunk, &offset).u64);
		return offset;

	default:
		printf(CONSOLE_FOREGROUND_BRIGHT_RED "UNKOWN OPCODE: '%d'\n"
			CONSOLE_COLOR_RESET, instruction);
		return offset + 1;
	}
}

uint64_t dis_print_simple_instruction(const char* name, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_GREEN "%-20s\n"
		CONSOLE_COLOR_RESET, name);
	return offset + 1;
}

uint64_t dis_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_BRIGHT_MAGENTA "%-20s "
		CONSOLE_FOREGROUND_CYAN "'%s'\n"
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(byte));
	return offset + 2;
}

uint64_t dis_print_2operand_instruction(const char* name, uint8_t first, uint8_t second, uint64_t offset)
{
	printf(CONSOLE_FOREGROUND_YELLOW "%-20s "
		CONSOLE_FOREGROUND_CYAN "'%s'" CONSOLE_COLOR_RESET ", "
		CONSOLE_FOREGROUND_CYAN "'%s'\n"
		CONSOLE_COLOR_RESET, name, BuiltinTypeIDToString(first), BuiltinTypeIDToString(second));
	return offset + 3;
}

void dis_print_jump_instruction(const char* name, uint32_t to_offset)
{
	printf(CONSOLE_FOREGROUND_YELLOW "%-20s "
		CONSOLE_FOREGROUND_CYAN "TO: '%"PRIu32"'\n"
		CONSOLE_COLOR_RESET, name, to_offset);
}

void dis_print_hex_instruction(const char* name, uint64_t value)
{
	printf(CONSOLE_FOREGROUND_BRIGHT_BLUE "%-20s "
		CONSOLE_FOREGROUND_BRIGHT_GREEN "'0x%"PRIX64"'\n"
		CONSOLE_COLOR_RESET, name, value);
}

void dis_print_global_instruction(const char* name, uint64_t byte_offset, const Chunk* chunk)
{
	uint64_t debug_offset = ChunkGetDEBUGSection(chunk);
	uint64_t var_nb = (byte_offset - ChunkGetGLOBALSection(chunk)) / sizeof(QWORD);
	
	if (debug_offset != 0)
	{
		// + sizeof(QWORD) as we need to offset by one QWORD to get the offset to the name
		const char* var_name = chunk->code + *(uint64_t*)(chunk->code + debug_offset + var_nb * 2 * sizeof(QWORD) + sizeof(QWORD));
		BuiltinTypeID id = (BuiltinTypeID)((QWORD*)(chunk->code + debug_offset + var_nb * 2 * sizeof(QWORD)))->u8;
		printf(CONSOLE_FOREGROUND_BRIGHT_BLUE "%-20s "
			CONSOLE_FOREGROUND_BRIGHT_GREEN "'%s'"
			CONSOLE_FOREGROUND_BRIGHT_CYAN " (%s)\n"
			CONSOLE_COLOR_RESET, name, var_name, BuiltinTypeIDToString(id));
	}
	else
	{
		printf(CONSOLE_FOREGROUND_BRIGHT_BLUE "%-20s\n"
			CONSOLE_COLOR_RESET, name);
	}

}