/** @file struct_table.c
* Contains the definitions of the functions declared in 'struct_table.h'
*/

#include "struct_table.h"

void ASTTableInit(ASTTable* table)
{
	VariableTableInit(&table->glob_table);
	StringTableInit(&table->str_table);
}

void ASTTableFree(ASTTable* table)
{
	StringTableFree(&table->str_table);
	VariableTableFree(&table->glob_table);
}

void StringTableInit(StringTable* table)
{
	table->insertion_alloc_capacity = 10;
	table->insertion_order = safe_malloc(sizeof(StringEntry*) * 10);
	table->capacity = 10;
	table->count = 0;
	table->all_str_size = 0;
	table->str_entries = safe_malloc(sizeof(StringEntry) * 10);
	for (size_t i = 0; i < table->capacity; i++)
		table->str_entries[i].key.ptr = NULL;
}

void StringTableFree(StringTable* table)
{
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (table->str_entries[i].key.ptr != NULL)
			StringFree(&table->str_entries[i].key);
	}
	safe_free(table->insertion_order);
	safe_free(table->str_entries);
}

void StringTableAdd(StringTable* table, const String* to_add)
{
	if ((double)table->count + 1 > (double)table->capacity * STRING_TABLE_MAX_LOAD)
	{
		string_table_grow_capacity(table, table->capacity * 2);
	}
	

	StringEntry* entry = string_table_find_entry(table->str_entries, table->capacity, StringToStringView(to_add));
	if (entry->key.ptr == NULL)
	{
		//Reallocate array
		if (table->count == table->insertion_alloc_capacity)
		{
			StringEntry** new_ptr = safe_malloc((table->insertion_alloc_capacity *= 2) * sizeof(StringEntry));
			memcpy(new_ptr, table->insertion_order, sizeof(StringEntry) * table->count);
			safe_free(table->insertion_order);
			table->insertion_order = new_ptr;
		}
		//Store pointer in order array
		table->insertion_order[table->count] = entry;
		entry->counter_nb = table->count++;
		table->all_str_size += to_add->size;
		StringCopy(&entry->key, to_add);
	}
	//Do not do anything if string already exists
}

void VariableTableInit(GlobalTable* table)
{
	table->capacity = 10;
	table->count = 0;
	table->entries = safe_malloc(sizeof(GlobalEntry) * 10);
	for (size_t i = 0; i < table->capacity; i++)
		table->entries[i].key.ptr = NULL;
}

void VariableTableFree(GlobalTable* table)
{
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (table->entries[i].key.ptr != NULL)
			StringFree(&table->entries[i].key);
	}
	safe_free(table->entries);
}

bool VariableTableGet(GlobalTable* table, StringView key, QWORD* value)
{
	if (table->count == 0)
		return false;

	const GlobalEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;

	*value = entry->value;
	return true;
}

bool VariableTableContains(GlobalTable* table, StringView key)
{
	const GlobalEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;
	return true;
}

bool VariableTableSet(GlobalTable* table, StringView strv, QWORD value, Type type)
{
	if ((double)table->count + 1 > (double)table->capacity * VARIABLE_TABLE_MAX_LOAD)
	{
		variable_table_grow_capacity(table, table->capacity * 2);
	}

	GlobalEntry* entry = variable_table_find_entry(table->entries, table->capacity, strv);
	bool is_new = (entry->key.ptr == NULL);
	if (is_new)
	{
		entry->counter_nb = table->count++;
	}
	
	StringViewToString(strv, &entry->key);
	entry->value = value;
	entry->type = type;

	return is_new;
}

bool VariableTableDelete(GlobalTable* table, StringView key)
{
	if (table->count == 0)
		return false;

	// Find the entry.
	GlobalEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;

	StringFree(&entry->key);
	entry->key.ptr = NULL;
	return true;
}

GlobalEntry* VariableTableGetEntry(GlobalTable* table, StringView key)
{
	GlobalEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return NULL;
	return entry;
}

uint64_t hash_strv(StringView strv)
{
	//FNV-1a hashing method
	//https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash

	//FNV_offset_basis
	uint64_t hash = 0xcbf29ce484222325;
	for (size_t i = 0; i < (uint64_t)(strv.end - strv.start); i++)
	{
		hash ^= (uint8_t)strv.start[i];
		hash *= 0x100000001b3; //FNV_prime
	}
	return hash;
}

void variable_table_grow_capacity(GlobalTable* table, uint64_t capacity)
{
	GlobalEntry* entries = safe_malloc(sizeof(GlobalEntry) * capacity);
	for (size_t i = 0; i < capacity; i++)
		entries[i].key.ptr = NULL;

	for (size_t i = 0; i < table->capacity; i++)
	{
		const GlobalEntry* entry = &table->entries[i];
		if (entry->key.ptr == NULL)
			continue;

		//Rehash
		GlobalEntry* dest = variable_table_find_entry(entries, capacity, StringToStringView(&entry->key));
		
		//Copy entry to new location
		memcpy(dest, entry, sizeof(GlobalEntry));
		//We need to fix the pointers as String have a small buffer optimization
		if (StringIsStackAllocated(&dest->key))
			dest->key.ptr = dest->key.buffer;
	}

	safe_free(table->entries);
	table->entries = entries;
	table->capacity = capacity;
}

void string_table_grow_capacity(StringTable* table, uint64_t capacity)
{
	StringEntry* entries = safe_malloc(sizeof(StringEntry) * capacity);
	for (size_t i = 0; i < capacity; i++)
		entries[i].key.ptr = NULL;

	for (size_t i = 0; i < table->capacity; i++)
	{
		const StringEntry* entry = &table->str_entries[i];
		if (entry->key.ptr == NULL)
			continue;

		StringEntry* dest = string_table_find_entry(entries, capacity, StringToStringView(&entry->key));
		
		table->insertion_order[entry->counter_nb] = dest;
		//Copy entry to new location
		memcpy(dest, entry, sizeof(StringEntry));
		
		//We need to fix the pointers as String have a small buffer optimization
		if (StringIsStackAllocated(&dest->key))
			dest->key.ptr = dest->key.buffer;
	}

	safe_free(table->str_entries);
	table->str_entries = entries;
	table->capacity = capacity;
}

StringEntry* string_table_find_entry(StringEntry* entries, uint64_t capacity, StringView strv)
{
	uint64_t index = hash_strv(strv) % capacity;
	for (;;)
	{
		StringEntry* entry = entries + index;
		if (entry->key.ptr == NULL)
			return entry;
		else if (StringViewEqual(StringToStringView(&entry->key), strv))
			return entry;

		index = (index + 1) % capacity;
	}
}

GlobalEntry* variable_table_find_entry(GlobalEntry* entries, uint64_t capacity, StringView strv)
{
	uint64_t index = hash_strv(strv) % capacity;
	for (;;)
	{
		GlobalEntry* entry = entries + index;
		if (entry->key.ptr == NULL)
			return entry;
		else if (StringViewEqual(StringToStringView(&entry->key), strv))
			return entry;

		index = (index + 1) % capacity;
	}
}
