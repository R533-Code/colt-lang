/** @file disassemble.h
* Contains helper for disassembling chunks, and getting human-readable descriptions of their content.
*/

#ifndef HG_COLTI_DISASSEMBLE
#define HG_COLTI_DISASSEMBLE

#include "chunk.h"

namespace colti::code
{
	/// @brief Prints a human readable description of the code contained in a chunk
	/// @param chunk The chunk whose content to print
	/// @param name The chunk name
	void disassemble(const Chunk& chunk, const char* name) noexcept;

	namespace details
	{
		/**********************************
		IMPLEMENTATION HELPERS
		**********************************/

		/// @brief Dispatches a code to the correct printing function
		/// @param chunk The chunk from which to extract the code
		/// @param offset The offset to the code
		/// @return Modified offset
		uint64_t impl_chunk_print_code(const Chunk& chunk, uint64_t offset) noexcept;

		/// @brief Prints a one byte instruction
		/// @param name The name of the instruction
		/// @param offset The current byte offset
		/// @return The current byte offset + 1
		uint64_t impl_print_simple_instruction(const char* name, uint64_t offset) noexcept;

		/// @brief Prints a one byte instruction followed by the byte following it
		/// @param name The name of the instruction
		/// @param byte The byte that follows it
		/// @param offset The current byte offset
		/// @return The current byte offset + 2
		uint64_t impl_print_byte_instruction(const char* name, uint8_t byte, uint64_t offset) noexcept;

		/// @brief Prints a one byte instruction followed by the operand following it
		/// @param name The name of the instruction
		/// @param byte The byte that follows it
		/// @param offset The current byte offset
		/// @return The current byte offset + 2
		uint64_t impl_print_operand_instruction(const char* name, uint8_t byte, uint64_t offset) noexcept;

		/// @brief Prints a one byte instruction followed by the int following it.
		/// There is no offset to pass to this function, but rather, the 'value' argument
		/// should be ChunkGetInt[16|32|64](..., &offset).
		/// @param name The name of the instruction
		/// @param value The int following the instruction
		void impl_print_int_instruction(const char* name, int64_t value) noexcept;

		/// @brief Prints a one byte instruction followed by the int following it.
		/// There is no offset to pass to this function, but rather, the 'value' argument
		/// should be ChunkGetUInt[16|32|64](..., &offset).
		/// @param name The name of the instruction
		/// @param value The int following the instruction
		void impl_print_hex_instruction(const char* name, uint64_t value) noexcept;
	}
}
#endif //HG_COLTI_DISASSEMBLE