/** @file disassemble.h
* Contains helper for disassembling chunks, and getting human-readable descriptions of their content.
*/

#ifndef HG_COLTI_DISASSEMBLE
#define HG_COLTI_DISASSEMBLE

#include "chunk.h"

/// @brief Prints a human readable description of the code contained in a chunk
/// @param chunk The chunk whose content to print
/// @param name The chunk name
void ChunkDisassemble(const Chunk* chunk, const char* name);

/**********************************
IMPLEMENTATION HELPERS
**********************************/

/// @brief Dispatches a code to the correct printing function
/// @param chunk The chunk from which to extract the code
/// @param offset The offset to the code
/// @return Modified offset
uint64_t impl_chunk_print_code(const Chunk* chunk, uint64_t offset);

/// @brief Prints a one byte instruction
/// @param name The name of the instruction
/// @param offset The current byte offset
/// @return The current byte offset + 1
uint64_t impl_print_simple_instruction(const char* name, uint64_t offset);

/// @brief Prints a one byte instruction followed by the byte following it
/// @param name The name of the instruction
/// @param byte The byte that follows it
/// @param offset The current byte offset
/// @return The current byte offset + 2
uint64_t impl_print_byte_instruction(const char* name, uint8_t byte, uint64_t offset);

/// @brief Prints a one byte instruction followed by the operand following it
/// @param name The name of the instruction
/// @param byte The byte that follows it
/// @param offset The current byte offset
/// @return The current byte offset + 2
uint64_t impl_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset);

/// @brief Prints a one byte instruction followed by the 2 operands following it
/// @param name The name of the instruction
/// @param first The first operand
/// @param second The second operand
/// @param offset The current byte offset
/// @return The current byte offset + 3
uint64_t impl_print_2operand_instruction(const char* name, uint8_t first, uint8_t second, uint64_t offset);

/// @brief Prints a one byte instruction followed by the int following it.
/// There is no offset to pass to this function, but rather, the 'value' argument
/// should be ChunkGetInt[16|32|64](..., &offset).
/// @param name The name of the instruction
/// @param value The int following the instruction
void impl_print_int_instruction(const char* name, int64_t value);

/// @brief Prints a one byte instruction followed by the int following it.
/// There is no offset to pass to this function, but rather, the 'value' argument
/// should be ChunkGetUInt[16|32|64](..., &offset).
/// @param name The name of the instruction
/// @param value The int following the instruction
void impl_print_hex_instruction(const char* name, uint64_t value);

#endif //HG_COLTI_DISASSEMBLE