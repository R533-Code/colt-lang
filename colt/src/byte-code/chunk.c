/** @file chunk.c
* Contains the definitions of the functions declared in 'chunk.h'
*/

#include "chunk.h"

void ChunkPrintBytes(const Chunk* chunk)
{
	for (uint32_t i = 0; i < chunk->count; i++)
	{
		if (i > 0) printf(" ");
		printf("%02X", chunk->code[i]);
	}
	printf("\n");
}

void ChunkInit(Chunk* chunk)
{	
	chunk->capacity = 128;
	chunk->count = 0;
	chunk->code = safe_malloc(128);

	QWORD zero = { .u64 = 0 };
	//CODE OFFSET
	ChunkWriteQWORD(chunk, zero);
	//GLOBAL OFFSET
	ChunkWriteQWORD(chunk, zero);
	//CONST OFFSET
	ChunkWriteQWORD(chunk, zero);
	//DEBUG OFFSET
	ChunkWriteQWORD(chunk, zero);
}

void ChunkWriteOpCode(Chunk* chunk, OpCode code)
{
	impl_chunk_write_byte(chunk, (uint8_t)code);
}

void ChunkWriteOperand(Chunk* chunk, BuiltinTypeID type)
{
	impl_chunk_write_byte(chunk, (uint8_t)type);
}

uint64_t ChunkWriteBYTE(Chunk* chunk, BYTE byte)
{
	impl_chunk_write_byte(chunk, byte.u8);
	return 1;
}

void ChunkWriteBytes(Chunk* chunk, const uint8_t* const bytes, uint32_t size)
{
	if (!(chunk->count + size < chunk->capacity)) //Grow if needed
		impl_chunk_grow_size(chunk, size);
	memcpy(chunk->code, bytes, size);
	chunk->count += size;
}

uint64_t ChunkWriteWORD(Chunk* chunk, WORD value)
{
	//We need to pad if needed
	uint64_t offset = (uint64_t)(chunk->code + chunk->count) & 1; //same as % 2
	if (!(chunk->count + offset + sizeof(uint16_t) < chunk->capacity)) //Grow if needed
		impl_chunk_grow_double(chunk);

	//Set the padding byte to CD on Debug build
	DO_IF_DEBUG_BUILD(if (offset != 0) chunk->code[chunk->count] = 205;);

	//Copy the bytes of the integer to the aligned memory
	memcpy(chunk->code + (chunk->count += offset), &value, sizeof(uint16_t));
	//We already added offset to 'count' ^
	chunk->count += sizeof(uint16_t);

	return offset;
}

uint64_t ChunkWriteDWORD(Chunk* chunk, DWORD value)
{
	uint64_t offset = (uint64_t)(chunk->code + chunk->count) & 3; //same as % 4
	if (!(chunk->count + offset + sizeof(uint32_t) < chunk->capacity)) //Grow if needed
		impl_chunk_grow_double(chunk);

	//Set the padding bytes to CD on Debug build
	DO_IF_DEBUG_BUILD(if (offset != 0) memset(chunk->code + chunk->count, 205, offset););

	//Copy the bytes of the integer to the aligned memory
	memcpy(chunk->code + (chunk->count += offset), &value, sizeof(uint32_t));
	//We already added offset to 'count' ^
	chunk->count += sizeof(uint32_t);
	
	return offset;
}

uint64_t ChunkWriteQWORD(Chunk* chunk, QWORD value)
{
	uint64_t offset = (uint64_t)(chunk->code + chunk->count) & 7; //same as % 8
	if (!(chunk->count + offset + sizeof(uint64_t) < chunk->capacity)) //Grow if needed
		impl_chunk_grow_double(chunk);

	//Set the padding bytes to CD on Debug build
	DO_IF_DEBUG_BUILD(if (offset != 0) memset(chunk->code + chunk->count, 205, offset););

	//Copy the bytes of the integer to the aligned memory
	memcpy(chunk->code + (chunk->count += offset), &value, sizeof(uint64_t));
	//We already added offset to 'count' ^
	chunk->count += sizeof(uint64_t);
	return offset;
}

BYTE ChunkGetBYTE(const Chunk* chunk, uint64_t* offset)
{
	*offset += 1;
	BYTE byte = { .u8 = chunk->code[(*offset)++] };
	return byte;
}

WORD ChunkGetWORD(const Chunk* chunk, uint64_t* offset)
{
	//Local variable which will be used to store a copy of the offset, than write only one time to *offset
	//As the offset points to OP_PUSH_WORD, we also need to add 1
	uint64_t local_offset = *offset + 1;
	//We add the padding to the offset, which means we are now pointing to the int16
	local_offset += ((uint64_t)(chunk->code + local_offset) & 1);

	//Extract the int16 from the bytes
	WORD return_val = { .u16 = *(int16_t*)(chunk->code + local_offset) };
	//Update the value of the offset
	*offset = local_offset + sizeof(int16_t);
	return return_val;
}

DWORD ChunkGetDWORD(const Chunk* chunk, uint64_t* offset)
{
	//Local variable which will be used to store a copy of the offset, than write only one time to *offset
	//As the offset points to OP_PUSH_DWORD, we also need to add 1
	uint64_t local_offset = *offset + 1;
	//We add the padding to the offset, which means we are now pointing to the int32
	local_offset += (uint64_t)(chunk->code + local_offset) & 3; //same as % 4


	DWORD return_val;
	return_val.u32 = *(int32_t*)(chunk->code + local_offset);
	//Update the value of the offset
	*offset = local_offset + sizeof(int32_t);
	return return_val;
}

QWORD ChunkGetQWORD(const Chunk* chunk, uint64_t* offset)
{
	//Local variable which will be used to store a copy of the offset, than write only one time to *offset
	//As the offset points to OP_PUSH_QWORD, we also need to add 1
	uint64_t local_offset = *offset + 1;
	//We add the padding to the offset, which means we are now pointing to the int64
	local_offset += (uint64_t)(chunk->code + local_offset) & 7; //same as % 8

	QWORD return_val;
	return_val.u64 = *(int64_t*)(chunk->code + local_offset);
	//Update the value of the offset
	*offset = local_offset + sizeof(int64_t);
	return return_val;
}

void ChunkFree(Chunk* chunk)
{
	safe_free(chunk->code);

	//Most functions that take a Chunk* check for if the capacity is 0,
	//which should never be.
	//By setting it to 0, we ensure that calling a function on the freed chunk
	//will cause an assertion on debug builds.
	DO_IF_DEBUG_BUILD(chunk->capacity = 0);
}

void ChunkReserve(Chunk* chunk, size_t more_byte_capacity)
{
	impl_chunk_grow_size(chunk, more_byte_capacity);
}

void ChunkSerialize(const Chunk* chunk, const char* path)
{
	FILE* file = fopen(path, "wb");
	if (file == NULL)
	{
		print_error_format("Could not create the file at path '%s'!", path);
		exit(EXIT_OS_RESOURCE_FAILURE);
	}
	//Write the binary code
	fwrite(chunk->code, sizeof(char), chunk->count, file);
	fclose(file);
}

Chunk ChunkDeserialize(const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == NULL)
	{
		print_error_format("Could not open the file '%s'!", path);
		exit(EXIT_OS_RESOURCE_FAILURE);
	}
	Chunk chunk;
	fseek(file, 0L, SEEK_END);
	size_t file_size = ftell(file); //Get file size
	chunk.code = safe_malloc(chunk.capacity = file_size);
	chunk.count = chunk.capacity;
	
	rewind(file); //Go back to the beginning of the file
	size_t bytes_read = fread(chunk.code, sizeof(char), file_size, file);
	fclose(file);
	if (bytes_read != file_size)
	{
		print_error_format("Could not read all the file's (at path '%s') content!", path);
		exit(EXIT_OS_RESOURCE_FAILURE);
	}
	return chunk;
}

BYTE unsafe_get_byte(uint8_t** ptr)
{
	BYTE return_val;
	return_val.u8 = *(*ptr);
	*ptr += sizeof(uint8_t);
	return return_val;
}

WORD unsafe_get_word(uint8_t** ptr)
{
	*ptr += (uint64_t)(*ptr) & 1; //read past padding
	WORD return_val;
	return_val.u16 = *((uint16_t*)*ptr);
	*ptr += sizeof(int16_t);
	return return_val;
}

DWORD unsafe_get_dword(uint8_t** ptr)
{
	*ptr += (uint64_t)(*ptr) & 3; //read past padding
	DWORD return_val;
	return_val.u32 = *((uint32_t*)*ptr);
	*ptr += sizeof(int32_t);
	return return_val;
}

QWORD unsafe_get_qword(uint8_t** ptr)
{
	*ptr += (uint64_t)(*ptr) & 7; //read past padding
	QWORD return_val;
	return_val.u64 = *((uint64_t*)*ptr);
	*ptr += sizeof(int64_t);
	return return_val;
}

void impl_chunk_grow_double(Chunk* chunk)
{
	colt_assert(chunk->capacity != 0, "Chunk capacity was 0! Be sure to call ChunkInit for any Chunk you create!");

	//Allocate double the capacity
	uint8_t* ptr = (uint8_t*)safe_malloc(chunk->capacity *= 2);
	//Copy byte-code to new location
	memcpy(ptr, chunk->code, chunk->count);
	
	safe_free(chunk->code);
	chunk->code = ptr;
}

void impl_chunk_grow_size(Chunk* chunk, size_t size)
{
	colt_assert(size != 0, "Tried to augment the capacity of a Chunk by 0!");
	colt_assert(chunk->capacity != 0, "Chunk capacity was 0! Be sure to call ChunkInit for any Chunk you create!");

	//Allocate the new capacity
	uint8_t* ptr = (uint8_t*)safe_malloc(chunk->capacity += size);
	//Copy byte-code to new location
	memcpy(ptr, chunk->code, chunk->count);

	safe_free(chunk->code);
	chunk->code = ptr;
}

void impl_chunk_write_byte(Chunk* chunk, uint8_t byte)
{
	if (chunk->count == chunk->capacity) //Grow if needed
		impl_chunk_grow_double(chunk);
	chunk->code[chunk->count] = byte;
	++chunk->count;
}