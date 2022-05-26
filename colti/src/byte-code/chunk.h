/** @file chunk.h
* Contains the Chunk struct, which is used to provide simpler operations for storing and writing byte-code.
* A Chunk can be seen as a contiguous dynamic array which handles resizes automatically
* when writing byte-code to it.
* While some functions might seem redundant (ChunkWriteOpCode, ChunkWriteOperand),
* they provide a bit of type safety (as C's type system can be summarized with warnings)
* over just appending a byte at the end of the Chunk's code.
* The ChunkWrite(BYTE|[DQ]?WORD) aligns the writing of the value to its required alignment,
* which is also the reason for which to use ChunkGet(BYTE|[DQ]?WORD).
* These take a pointer to an int representing the offset, as it's the functions responsibility
* to update that offset.
* The unsafe_get_... are used for when a pointer is used rather than an offset.
*/

#ifndef HG_COLTI_CHUNK
#define HG_COLTI_CHUNK

#include "common.h"
#include "byte_code.h" //Contains the byte-code enum

namespace colti::code
{

	/// @brief Represents a stream of instructions
	class Chunk
	{
		/// @brief Number of items pointed to
		uint64_t count;
		/// @brief Capacity of the current allocated code_buffer
		uint64_t capacity;

		/// @brief Pointer to the beginning of the byte-code
		uint8_t* code;

	public: //CONSTRUCTORS AND DESTRUCTOR
		
		/// @brief Zero-initializes a chunk
		Chunk() noexcept;

		/// @brief De-serializes a chunk from a file
		/// @param path The path to the file from which to de-serialize
		Chunk(const char* path) noexcept;

		/// @brief Frees memory used by a chunk
		~Chunk() noexcept;

	public: //METHODS

		/// @brief Prints the byte content of a Chunk
		void printBytes() noexcept;

		/// @brief Appends an OpCode to the end of the chunk
		/// @param code The byte to append
		void writeOpCode(OpCode code) noexcept;

		/// @brief Appends multiple bytes to the end of a chunk
		/// @param bytes The array of bytes from which to copy the bytes
		/// @param size The number of bytes to copy from 'bytes'
		void writeBytes(const uint8_t* const bytes, uint64_t size) noexcept;


		/// @brief Appends an OperandType to the end of the chunk
		/// @param type The type to append
		void writeOperand(OperandType type) noexcept;

		/// @brief Appends a BYTE to the end of the chunk
		/// @param byte The byte to append
		void writeBYTE(BYTE value) noexcept;

		/// @brief Writes a WORD to the end of a chunk, padding if necessary
		/// @param value The value to write
		void writeWORD(WORD value) noexcept;

		/// @brief Writes a DWORD to the end of a chunk, padding if necessary
		/// @param value The value to write
		void writeDWORD(DWORD value) noexcept;
		
		/// @brief Writes a QWORD to the end of a chunk, padding if necessary
		/// @param value The value to write
		void writeQWORD(QWORD value) noexcept;
	
		/// @brief Gets a byte from the offset specified
		/// @param offset The offset should point to the OP_IMMEDIATE_BYTE, is modified by this function
		/// @return The byte at that offset
		BYTE getBYTE(uint64_t& offset) noexcept;

		/// @brief Gets a word from the offset specified, aligning the access
		/// @param offset The offset should point to the OP_IMMEDIATE_WORD, is modified by this function
		/// @return The word at that offset
		WORD getWORD(uint64_t& offset) noexcept;

		/// @brief Gets a double word from the offset specified, aligning the access
		/// @param offset The offset should point to the OP_IMMEDIATE_DWORD
		/// @return The double word at that offset
		DWORD getDWORD(uint64_t& offset) noexcept;

		/// @brief Gets a quad word from the offset specified, aligning the access
		/// @param offset The offset should point to the OP_IMMEDIATE_QWORD
		/// @return The quad word at that offset
		QWORD getQWORD(uint64_t& offset) noexcept;

		/// @brief Heap allocates 'size' MORE bytes to the current capacity of the Chunk
		/// @param more_byte_capacity The count of bytes to add to the capacity
		void reserve_more(size_t more_byte_capacity) noexcept;

		/// @brief Serializes a chunk to a file
		/// @param path The path to the file to which to serialize
		void serialize(const char* path) noexcept;

	private: //IMPLEMENTATION HELPERS
		/// @brief Doubles the capacity of a chunk
		void impl_chunk_grow_double() noexcept;

		/// @brief Augments the capacity of a chunk by 'size'
		/// @param size The capacity to add
		void impl_chunk_grow_size(size_t size) noexcept;

		/// @brief Appends a byte at the end of the chunk
		/// @param byte The byte to append
		void impl_chunk_write_byte(uint8_t byte) noexcept;
	};
}



/**********************************
IMPLEMENTATION HELPERS
**********************************/

/// @brief Extracts a BYTE from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_IMMEDIATE_BYTE (not checked)
/// @return BYTE union representing the read byte
BYTE unsafe_get_byte(uint8_t** ptr);

/// @brief Extracts a WORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_IMMEDIATE_WORD (not checked)
/// @return WORD union representing the read word
WORD unsafe_get_word(uint8_t** ptr);

/// @brief Extracts a DWORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_IMMEDIATE_DWORD (not checked)
/// @return DWORD union representing the read word
DWORD unsafe_get_dword(uint8_t** ptr);

/// @brief Extracts a QWORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_IMMEDIATE_QWORD(not checked)
/// @return QWORD union representing the read word
QWORD unsafe_get_qword(uint8_t** ptr);

#endif //HG_COLTI_CHUNK