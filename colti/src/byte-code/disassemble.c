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
		colti_assert(offset + 1 <= chunk->count, "Missing byte after OP_IMMEDIATE_BYTE!");
		impl_print_hex_instruction("OP_IMMEDIATE_BYTE", ChunkGetBYTE(chunk, &offset).ui8);
		return offset;

	case OP_IMMEDIATE_WORD:
		colti_assert(offset + ((uint64_t)(chunk->code + offset) & 1) + sizeof(int16_t) <= chunk->count, "Missing int16 after OP_IMMEDIATE_WORD");
		impl_print_hex_instruction("OP_IMMEDIATE_WORD", ChunkGetWORD(chunk, &offset).ui16);
		return offset;

	case OP_IMMEDIATE_DWORD:
		colti_assert(offset + ((uint64_t)(chunk->code + offset) % 4) + sizeof(int32_t) <= chunk->count, "Missing int32 after OP_IMMEDIATE_DWORD");
		impl_print_hex_instruction("OP_IMMEDIATE_DWORD", ChunkGetDWORD(chunk, &offset).ui32);
		return offset;

	case OP_IMMEDIATE_QWORD:
		colti_assert(offset + ((uint64_t)(chunk->code + offset) % 8) + sizeof(int64_t) <= chunk->count, "Missing int64 after OP_IMMEDIATE_QWORD");
		impl_print_hex_instruction("OP_IMMEDIATE_QWORD", ChunkGetQWORD(chunk, &offset).ui64);
		return offset;

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

		/******************************************************/

	case OP_PRINT:
		return impl_print_operand_instruction("OP_PRINT", chunk->code[offset + 1], offset);

		/******************************************************/

	case OP_RETURN:
		return impl_print_simple_instruction("OP_RETURN", offset);

	default:
		printf("UNKOWN OPCODE: '%d'\n", instruction);
		return offset + 1;
	}
}

uint64_t impl_print_simple_instruction(const char* name, uint64_t offset)
{
	printf("%s\n", name);
	return offset + 1;
}

uint64_t impl_print_byte_instruction(const char* name, uint8_t byte, uint64_t offset)
{
	printf("%s '%d'\n", name, byte);
	return offset + 2;
}

uint64_t impl_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset)
{
	const char* operand;
	switch (byte)
	{
	break; case COLTI_BOOL_ID:		operand = "BOOL";
	break; case COLTI_DOUBLE_ID:	operand = "DOUBLE";
	break; case COLTI_FLOAT_ID:	operand = "FLOAT";
	break; case COLTI_I8_ID:		operand = "INT8";
	break; case COLTI_I16_ID:		operand = "INT16";
	break; case COLTI_I32_ID:		operand = "INT32";
	break; case COLTI_I64_ID:		operand = "INT64";
	break; case COLTI_U8_ID:		operand = "UINT8";
	break; case COLTI_U16_ID:		operand = "UINT16";
	break; case COLTI_U32_ID:		operand = "UINT32";
	break; case COLTI_U64_ID:		operand = "UINT64";
	break; default:						operand = "UNKOWN";
	}
	printf("%s '%s'\n", name, operand);
	return offset + 2;
}

void impl_print_int_instruction(const char* name, int64_t value)
{
	printf("%s '%"PRId64"'", name, value);
}

void impl_print_hex_instruction(const char* name, uint64_t value)
{
	printf("%s '0x%"PRIX64"'\n", name, value);
}