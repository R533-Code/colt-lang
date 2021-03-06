/** @file disassemble.h
* Contains helper for disassembling chunks, and getting human-readable descriptions of their content.
*/

#ifndef HG_COLT_DISASSEMBLE
#define HG_COLT_DISASSEMBLE

#include "chunk.h"
#include "byte_code.h"

/// @brief Prints a human readable description of the code contained in a chunk
/// @param chunk The chunk whose content to print
/// @param name The chunk name
void ChunkDisassemble(const Chunk* chunk, const char* name);

/**********************************
IMPLEMENTATION HELPERS
**********************************/

#ifndef COLT_NO_DISASSEMBLY

/// @brief Writes a string to 'stdout', highlighting escape characters, and quoting it
/// @param str The string to print
void print_lstring(const char* str);

/// @brief Writes the value of a global value and its type
/// @param chunk The Chunk from which to read
/// @param var_nb The current global variable counter
void unsafe_print_global_variable(const Chunk* chunk, uint64_t var_nb);

/// @brief Dispatches a code to the correct printing function
/// @param chunk The chunk from which to extract the code
/// @param offset The offset to the code
/// @return Modified offset
uint64_t dis_chunk_print_code(const Chunk* chunk, uint64_t offset);

/// @brief Prints a one byte instruction
/// @param name The name of the instruction
/// @param offset The current byte offset
/// @return The current byte offset + 1
uint64_t dis_print_simple_instruction(const char* name, uint64_t offset);

/// @brief Prints a one byte instruction followed by the operand following it
/// @param name The name of the instruction
/// @param byte The byte that follows it
/// @param offset The current byte offset
/// @return The current byte offset + 2
uint64_t dis_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset);

/// @brief Prints a one byte instruction followed by the 2 operands following it
/// @param name The name of the instruction
/// @param first The first operand
/// @param second The second operand
/// @param offset The current byte offset
/// @return The current byte offset + 3
uint64_t dis_print_2operand_instruction(const char* name, uint8_t first, uint8_t second, uint64_t offset);

/// @brief Prints a short jump instruction, which is followed by a type operand, and a byte offset
/// @param name The name of the instruction
/// @param to_offset The offset value
void dis_print_jump_instruction(const char* name, uint32_t to_offset);

/// @brief Prints a one byte instruction followed by the int following it.
/// There is no offset to pass to this function, but rather, the 'value' argument
/// should be ChunkGetUInt[16|32|64](..., &offset).
/// @param name The name of the instruction
/// @param value The int following the instruction
void dis_print_hex_instruction(const char* name, uint64_t value);

/// @brief Prints a global variable name for read/write OpCodes
/// @param name The name of the instruction
/// @param byte_offset The offset of the QWORD (from ChunkGetQWORD)
/// @param chunk The Chunk from which to read debug data
void dis_print_global_instruction(const char* name, uint64_t byte_offset, const Chunk* chunk);

#endif

#endif //HG_COLT_DISASSEMBLE