/** @file chunk.h
* Contains the Chunk struct, which is used to provide simpler operations for storing and writing byte-code.
* A Chunk can be seen as a contiguous dynamic array which handles resizes automatically
* when writing byte-code to it.
* A Chunk usually contains 5 sections:
* - The HEADER: 4 uint64_t giving the offsets of the 4 other sections
*	- The offsets are in bytes, which should be added to Chunk.code
* - The GLOBAL pool: contains all modifiable global variables
* - The CONST pool: contains all non-modifiable const variables
* - The STRING pool: contains all string literals
* - The DEBUG pool: contains type informations for both const and global variables
* - The CODE section: contains the byte-code to execute
* While some functions might seem redundant (ChunkWriteOpCode, ChunkWriteOperand),
* they provide a bit of type safety (as C's type system can be summarized with warnings)
* over just appending a byte at the end of the Chunk's code.
* The ChunkWrite(BYTE|[DQ]?WORD) aligns the writing of the value to its required alignment,
* which is also the reason for which to use ChunkGet(BYTE|[DQ]?WORD).
* These take a pointer to an int representing the offset, as it's the functions responsibility
* to update that offset.
* The unsafe_get_... are used for when a pointer is used rather than an offset.
*/

#ifndef HG_COLT_CHUNK
#define HG_COLT_CHUNK

#include "common.h"
#include "byte_code.h" //Contains the byte-code enum

/// @brief Represents a stream of instructions
typedef struct
{
	/// @brief Number of items pointed to
	uint64_t count;
	/// @brief Capacity of the current allocated code_buffer
	uint64_t capacity;

	/// @brief Pointer to the beginning of the byte-code
	uint8_t* code;
} Chunk;

/// @brief Zero-initializes a chunk, and writes its header
/// @param chunk The chunk to initialize
void ChunkInit(Chunk* chunk);

/// @brief Appends an OpCode to the end of the chunk
/// @param chunk The chunk to append to
/// @param code The byte to append
void ChunkWriteOpCode(Chunk* chunk, OpCode code);

/// @brief Appends an BuiltinTypeID to the end of the chunk
/// @param chunk The chunk to append to
/// @param type The type to append
void ChunkWriteOperand(Chunk* chunk, BuiltinTypeID type);

/// @brief Appends a byte to the end of the chunk
/// @param chunk The chunk to append to
/// @param byte The byte to append
/// @return The padding used (which is always 1)
uint64_t ChunkWriteBYTE(Chunk* chunk, BYTE byte);

/// @brief Appends multiple bytes to the end of a chunk
/// @param chunk The chunk to append to
/// @param bytes The array of bytes from which to copy the bytes
/// @param size The number of bytes to copy from 'bytes'
void ChunkWriteBytes(Chunk* chunk, const uint8_t* const bytes, uint32_t size);

/// @brief Writes an int16 to the end of a chunk, padding if necessary
/// @param chunk The chunk to append to
/// @param value The value to write
/// @return The padding used
uint64_t ChunkWriteWORD(Chunk* chunk, WORD value);

/// @brief Writes an int32 to the end of a chunk, padding if necessary
/// @param chunk The chunk to append to
/// @param value The value to write
/// @return The padding used
uint64_t ChunkWriteDWORD(Chunk* chunk, DWORD value);

/// @brief Writes an int64 to the end of a chunk, padding if necessary
/// @param chunk The chunk to append to
/// @param value The value to write
/// @return The padding used
uint64_t ChunkWriteQWORD(Chunk* chunk, QWORD value);

/// @brief Gets a byte from the offset specified
/// @param chunk The chunk to get the value from
/// @param offset The offset should point to the OP_PUSH_BYTE, is modified by this function
/// @return The byte at that offset
BYTE ChunkGetBYTE(const Chunk* chunk, uint64_t* offset);

/// @brief Gets a word from the offset specified, aligning the access
/// @param chunk The chunk to get the value from
/// @param offset The offset should point to the OP_PUSH_WORD, is modified by this function
/// @return The word at that offset
WORD ChunkGetWORD(const Chunk* chunk, uint64_t* offset);

/// @brief Gets a double word from the offset specified, aligning the access
/// @param chunk The chunk to get the value from
/// @param offset The offset should point to the OP_PUSH_DWORD
/// @return The double word at that offset
DWORD ChunkGetDWORD(const Chunk* chunk, uint64_t* offset);

/// @brief Gets a quad word from the offset specified, aligning the access
/// @param chunk The chunk to get the value from
/// @param offset The offset should point to the OP_PUSH_QWORD
/// @return The quad word at that offset
QWORD ChunkGetQWORD(const Chunk* chunk, uint64_t* offset);

/// @brief Frees memory used by a chunk
/// @param chunk The chunk to free
void ChunkFree(Chunk* chunk);

/// @brief Heap allocates 'size' MORE bytes to the current capacity of the Chunk
/// @param chunk The chunk to modify
/// @param more_byte_capacity The count of bytes to add to the capacity
void ChunkReserve(Chunk* chunk, size_t more_byte_capacity);

/// @brief Serializes a chunk to a file
/// @param chunk The chunk to serialize
/// @param path The path to the file to which to serialize
void ChunkSerialize(const Chunk* chunk, const char* path);

/// @brief De-serializes a chunk from a file
/// @param path The path to the file from which to de-serialize
/// @return The de-serialized chunk
Chunk ChunkDeserialize(const char* path);

/**********************************
IMPLEMENTATION HELPERS
**********************************/

/// @brief Extracts a BYTE from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_PUSH_BYTE (not checked)
/// @return BYTE union representing the read byte
BYTE unsafe_get_byte(uint8_t** ptr);

/// @brief Extracts a WORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_PUSH_WORD (not checked)
/// @return WORD union representing the read word
WORD unsafe_get_word(uint8_t** ptr);

/// @brief Extracts a DWORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_PUSH_DWORD (not checked)
/// @return DWORD union representing the read word
DWORD unsafe_get_dword(uint8_t** ptr);

/// @brief Extracts a QWORD from a pointer and updates the location pointed by that pointer
/// @param ptr Pointer to the pointer pointing to the byte following OP_PUSH_QWORD(not checked)
/// @return QWORD union representing the read word
QWORD unsafe_get_qword(uint8_t** ptr);

/// @brief Doubles the capacity of a chunk
/// @param chunk The chunk to modify
void impl_chunk_grow_double(Chunk* chunk);

/// @brief Augments the capacity of a chunk by 'size'
/// @param chunk The chunk to modify
/// @param size The capacity to add
void impl_chunk_grow_size(Chunk* chunk, size_t size);

/// @brief Appends a byte at the end of the chunk
/// @param chunk The chunk to modify
/// @param byte The byte to append
void chunk_write_byte(Chunk* chunk, uint8_t byte);

#endif //HG_COLT_CHUNK