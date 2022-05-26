/** @file chunk.c
* Contains the definitions of the functions declared in 'chunk.h'
*/

#include "chunk.h"

namespace colti::code
{
	Chunk::Chunk() noexcept
		: capacity(32), count(0), code(safe_malloc(32))
	{}

	Chunk::~Chunk() noexcept
	{
		safe_free(chunk->code);
		//Most functions that take a Chunk* check for if the capacity is 0,
		//which should never be.
		//By setting it to 0, we ensure that calling a function on the freed chunk
		//will cause an assertion on debug builds.
		DO_IF_DEBUG_BUILD(chunk->capacity = 0);
	}

	Chunk::Chunk(const char* path) noexcept
		: capacity(0), count(0), code(nullptr)
	{
		FILE* file = fopen(path, "rb");
		if (file == NULL)
		{
			print_error_format("Could not open the file '%s'!", path);
			exit(EXIT_OS_RESOURCE_FAILURE);
		}

		fseek(file, 0L, SEEK_END);
		size_t file_size = ftell(file); //Get file size
		this->code = safe_malloc(chunk->capacity = file_size);
		this->count = this->capacity;
		
		rewind(file); //Go back to the beginning of the file
		size_t bytes_read = fread(chunk->code, sizeof(char), file_size, file);
		fclose(file);
		if (bytes_read != file_size)
		{
			print_error_format("Could not read all the file's (at path '%s') content!", path);
			exit(EXIT_OS_RESOURCE_FAILURE);
		}
	}

	void Chunk::printBytes() noexcept
	{
		for (uint32_t i = 0; i < this->count; i++)
		{
			if (i > 0) printf(" ");
			printf("%02X", this->code[i]);
		}
		printf("\n");
	}

	void Chunk::writeOpCode(OpCode code) noexcept
	{
		this->impl_chunk_write_byte(static_cast<uint8_t>(code));
	}

	void Chunk::writeOperand(OperandType type) noexcept
	{
		this->impl_chunk_write_byte(static_cast<uint8_t>(type));
	}

	void Chunk::writeBytes(const uint8_t* const bytes, uint32_t size) noexcept
	{
		colti_assert(bytes != nullptr, "bytes cannot be null!");
		if (!(this->count + size < this->capacity)) //Grow if needed
			this->impl_chunk_grow_size(size);
		memcpy(this->code, bytes, size);
		this->count += size;
	}

	void Chunk::writeBYTE(BYTE byte) noexcept
	{
		this->impl_chunk_write_byte(byte.ui8);
	}

	void Chunk::writeWORD(WORD value) noexcept
	{
		//We need to pad if needed
		uint64_t offset = static_cast<uint64_t>((this->code + this->count) & 1); //same as % 2
		if (!(this->count + offset + sizeof(uint16_t) < this->capacity)) //Grow if needed
			this->impl_chunk_grow_double();

		//Set the padding byte to CD on Debug build
		DO_IF_DEBUG_BUILD(if (offset != 0) this->code[this->count] = 205;);

		//Copy the bytes of the integer to the aligned memory
		memcpy(this->code + (this->count += offset), &value, sizeof(uint16_t));
		//We already added offset to 'count' ^
		this->count += sizeof(uint16_t);
	}

	void Chunk::writeDWORD(DWORD value) noexcept
	{
		uint64_t offset = static_cast<uint64_t>((this->code + this->count) % 4);
		if (!(this->count + offset + sizeof(uint32_t) < this->capacity)) //Grow if needed
			this->impl_this_grow_double();

		//Set the padding bytes to CD on Debug build
		DO_IF_DEBUG_BUILD(if (offset != 0) memset(this->code + this->count, 205, offset););

		//Copy the bytes of the integer to the aligned memory
		memcpy(this->code + (this->count += offset), &value, sizeof(uint32_t));
		//We already added offset to 'count' ^
		this->count += sizeof(uint32_t);
	}

	void Chunk::writeQWORD(QWORD value) noexcept
	{
		uint64_t offset = static_cast<uint64_t>((this->code + this->count) % 8);
		if (!(this->count + offset + sizeof(uint64_t) < this->capacity)) //Grow if needed
			this->impl_this_grow_double();

		//Set the padding bytes to CD on Debug build
		DO_IF_DEBUG_BUILD(if (offset != 0) memset(this->code + this->count, 205, offset););

		//Copy the bytes of the integer to the aligned memory
		memcpy(this->code + (this->count += offset), &value, sizeof(uint64_t));
		//We already added offset to 'count' ^
		this->count += sizeof(uint64_t);
	}

	BYTE Chunk::getBYTE(uint64_t& offset) noexcept
	{
		colti_assert(this->code[offset] == OP_IMMEDIATE_BYTE, "'offset' should point to an OP_IMMEDIATE_BYTE!");
		offset += 1;
		BYTE byte = { .ui8 = this->code[offset++] };
		return byte;
	}

	WORD Chunk::getWORD(uint64_t& offset) noexcept
	{
		colti_assert(this->code[offset] == OP_IMMEDIATE_WORD, "'offset' should point to an OP_IMMEDIATE_WORD!");

		//Local variable which will be used to store a copy of the offset, than write only one time to *offset
		//As the offset points to OP_IMMEDIATE_WORD, we also need to add 1
		uint64_t local_offset = offset + 1;
		//We add the padding to the offset, which means we are now pointing to the int16
		local_offset += static_cast<uint64_t>((this->code + local_offset) & 1);

		//Extract the int16 from the bytes
		WORD return_val = { .ui16 = *static_cast<int16_t*>(this->code + local_offset) };
		//Update the value of the offset
		offset = local_offset + sizeof(int16_t);
		return return_val;
	}
	
	DWORD Chunk::getDWORD(uint64_t& offset) noexcept
	{
		colti_assert(this->code[offset] == OP_IMMEDIATE_DWORD, "'offset' should point to an OP_IMMEDIATE_DWORD!");

		//Local variable which will be used to store a copy of the offset, than write only one time to offset
		//As the offset points to OP_IMMEDIATE_DWORD, we also need to add 1
		uint64_t local_offset = offset + 1;
		//We add the padding to the offset, which means we are now pointing to the int32
		local_offset += static_cast<uint64_t>((this->code + local_offset) % 4);


		DWORD return_val;
		return_val.ui32 = *static_cast<int32_t*>(this->code + local_offset);
		//Update the value of the offset
		offset = local_offset + sizeof(int32_t);
		return return_val;
	}

	QWORD Chunk::getQWORD(uint64_t& offset) noexcept
	{
		colti_assert(this->code[offset] == OP_IMMEDIATE_QWORD, "'offset' should point to an OP_IMMEDIATE_QWORD!");

		//Local variable which will be used to store a copy of the offset, than write only one time to offset
		//As the offset points to OP_IMMEDIATE_QWORD, we also need to add 1
		uint64_t local_offset = offset + 1;
		//We add the padding to the offset, which means we are now pointing to the int64
		local_offset += static_cast<uint64_t>((this->code + local_offset) % 8);

		QWORD return_val;
		return_val.ui64 = *static_cast<int64_t*>(this->code + local_offset);
		//Update the value of the offset
		offset = local_offset + sizeof(int64_t);
		return return_val;
	}

	void Chunk::reserveMore(size_t more_byte_capacity) noexcept
	{
		this->impl_chunk_grow_size(more_byte_capacity);
	}

	void Chunk::serialize(const char* path) noexcept
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

	void Chunk::impl_this_grow_double() noexcept
	{
		colti_assert(this->capacity != 0, "Chunk capacity was 0! Be sure to call ChunkInit for any Chunk you create!");

		//Allocate double the capacity
		uint8_t* ptr = static_cast<uint8_t*>(safe_malloc(this->capacity *= 2));
		//Copy byte-code to new location
		memcpy(ptr, this->code, this->count);
		
		safe_free(this->code);
		this->code = ptr;
	}

	void Chunk::impl_this_grow_size(size_t size) noexcept
	{
		colti_assert(size != 0, "Tried to augment the capacity of a Chunk by 0!");
		colti_assert(this->capacity != 0, "Chunk capacity was 0! Be sure to call ChunkInit for any Chunk you create!");

		//Allocate the new capacity
		uint8_t* ptr = static_cast<uint8_t*>(safe_malloc(this->capacity += size));
		//Copy byte-code to new location
		memcpy(ptr, this->code, this->count);

		safe_free(this->code);
		this->code = ptr;
	}

	void Chunk::impl_this_write_byte(uint8_t byte) noexcept
	{
		if (this->count == this->capacity) //Grow if needed
			impl_this_grow_double(this);
		this->code[this->count] = byte;
		++this->count;
	}

	BYTE Chunk::unsafe_get_byte(uint8_t*& ptr) noexcept
	{
		BYTE return_val;
		return_val.ui8 = *(ptr);
		ptr += sizeof(uint8_t);
		return return_val;
	}

	WORD Chunk::unsafe_get_word(uint8_t*& ptr) noexcept
	{
		ptr += reinterpret_cast<uint8_t*>(static_cast<uint64_t>(ptr) & 1); //read past padding
		WORD return_val;
		return_val.ui16 = *(reinterpret_cast<uint16_t*>(ptr));
		ptr += sizeof(int32_t);
		return return_val;
	}

	DWORD Chunk::unsafe_get_dword(uint8_t*& ptr) noexcept
	{
		ptr += reinterpret_cast<uint8_t*>(static_cast<uint64_t>(ptr) % 4); //read past padding
		DWORD return_val;
		return_val.ui32 = *(reinterpret_cast<uint32_t*>(ptr));
		ptr += sizeof(int32_t);
		return return_val;
	}

	QWORD Chunk::unsafe_get_qword(uint8_t*& ptr) noexcept
	{
		ptr += reinterpret_cast<uint8_t*>(static_cast<uint64_t>(ptr) % 8); //read past padding
		QWORD return_val;
		return_val.ui64 = *(reinterpret_cast<uint64_t*>(ptr));
		ptr += sizeof(int64_t);
		return return_val;
	}
}