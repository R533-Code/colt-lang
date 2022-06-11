/** @file struct_table.c
* Contains the definitions of the functions declared in 'struct_table.h'
*/

#include "struct_table.h"

void StringTableInit(StringTable* table)
{
	table->capacity = 10;
	table->count = 0;
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
	safe_free(table->str_entries);
}

void StringTableAdd(StringTable* table, String* to_add)
{
	if ((double)table->count + 1 > (double)table->capacity * VARIABLE_TABLE_MAX_LOAD)
	{
		string_table_grow_capacity(table, table->capacity * 2);
	}

	StringEntry* entry = string_table_find_entry(table->str_entries, table->capacity, StringToStringView(to_add));
	if (entry->key.ptr == NULL)
		table->count++;
	else //If String already exists, do not do anything
		return;

	StringCopy(&entry->key, to_add);
}

void VariableTableInit(VariableTable* table)
{
	table->global_counter = 0;
	table->capacity = 10;
	table->count = 0;
	table->entries = safe_malloc(sizeof(VariableEntry) * 10);
	for (size_t i = 0; i < table->capacity; i++)
		table->entries[i].key.ptr = NULL;
}

void VariableTableFree(VariableTable* table)
{
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (table->entries[i].key.ptr != NULL)
			StringFree(&table->entries[i].key);
	}
	safe_free(table->entries);
}

bool VariableTableGet(VariableTable* table, StringView key, QWORD* value)
{
	if (table->count == 0)
		return false;

	const VariableEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;

	*value = entry->value;
	return true;
}

bool VariableTableContains(VariableTable* table, StringView key)
{
	const VariableEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;
	return true;
}

bool VariableTableSet(VariableTable* table, StringView strv, QWORD value, Type type)
{
	if ((double)table->count + 1 > (double)table->capacity * VARIABLE_TABLE_MAX_LOAD)
	{
		variable_table_grow_capacity(table, table->capacity * 2);
	}

	VariableEntry* entry = variable_table_find_entry(table->entries, table->capacity, strv);
	bool is_new = (entry->key.ptr == NULL);
	if (is_new)
	{	
		entry->counter_nb = table->global_counter++; //update both global_counter and count
		table->count++;
	}
	
	StringViewToString(strv, &entry->key);
	entry->value = value;
	entry->type = type;
	return is_new;
}

bool VariableTableDelete(VariableTable* table, StringView key)
{
	if (table->count == 0) return false;

	// Find the entry.
	VariableEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return false;

	StringFree(&entry->key);
	entry->key.ptr = NULL;
	return true;
}

VariableEntry* VariableTableGetEntry(VariableTable* table, StringView key)
{
	VariableEntry* entry = variable_table_find_entry(table->entries, table->capacity, key);
	if (entry->key.ptr == NULL)
		return NULL;
	return entry;
}

void TablePrint(const VariableTable* table)
{
	fputs("============ TABLE ============\n", stdout);
	if (table->count == 0)
		fputs("!EMPTY!\n", stdout);
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (table->entries[i].key.ptr == NULL)
			continue;
		printf("%s: typeof(%.*s)\n", table->entries[i].key.ptr,
			(uint32_t)(table->entries[i].type.name.end - table->entries[i].type.name.start), table->entries[i].type.name.start
		);
		OpCode_Print(table->entries[i].value, (BuiltinTypeID)table->entries[i].type.type_id);
	}

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

void variable_table_grow_capacity(VariableTable* table, uint64_t capacity)
{
	VariableEntry* entries = safe_malloc(sizeof(VariableEntry) * capacity);
	for (size_t i = 0; i < capacity; i++)
		entries[i].key.ptr = NULL;

	for (size_t i = 0; i < table->capacity; i++)
	{
		const VariableEntry* entry = &table->entries[i];
		if (entry->key.ptr == NULL)
			continue;

		VariableEntry* dest = variable_table_find_entry(entries, capacity, StringToStringView(&entry->key));
		dest->key = entry->key;
		dest->value = entry->value;
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
		dest->key = entry->key;
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

VariableEntry* variable_table_find_entry(VariableEntry* entries, uint64_t capacity, StringView strv)
{
	uint64_t index = hash_strv(strv) % capacity;
	for (;;)
	{
		VariableEntry* entry = entries + index;
		if (entry->key.ptr == NULL)
			return entry;
		else if (StringViewEqual(StringToStringView(&entry->key), strv))
			return entry;

		index = (index + 1) % capacity;
	}
}
