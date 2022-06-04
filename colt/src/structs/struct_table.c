/** @file parse_args.c
* Contains the definitions of the functions declared in 'parse_args.h'
*/

#include "struct_table.h"

void TableInit(Table* table)
{
	table->capacity = 10;
	table->count = 0;
	table->entries = safe_malloc(sizeof(Entry) * 10);
	for (size_t i = 0; i < table->capacity; i++)
		table->entries[i].key.start = NULL;
}

void TableFree(Table* table)
{
	safe_free(table->entries);
}

bool TableGet(Table* table, StringView key, QWORD* value)
{
	if (table->count == 0)
		return false;

	const Entry* entry = table_find_entry(table->entries, table->capacity, key);
	if (entry->key.start == NULL)
		return false;

	*value = entry->value;
	return true;
}

bool TableContains(Table* table, StringView key)
{
	const Entry* entry = table_find_entry(table->entries, table->capacity, key);
	if (entry->key.start == NULL)
		return false;
	return true;
}

bool TableSet(Table* table, StringView strv, QWORD value, Type type)
{
	if ((double)table->count + 1 > (double)table->capacity * TABLE_MAX_LOAD)
	{
		table_grow_capacity(table, table->capacity * 2);
	}

	Entry* entry = table_find_entry(table->entries, table->capacity, strv);
	bool is_new = (entry->key.start == NULL);
	if (is_new)
		table->count++;

	entry->key = strv;
	entry->value = value;
	entry->type = type;
	return is_new;
}

bool TableDelete(Table* table, StringView key)
{
	if (table->count == 0) return false;

	// Find the entry.
	Entry* entry = table_find_entry(table->entries, table->capacity, key);
	if (entry->key.start == NULL)
		return false;

	entry->key.start = NULL;
	DO_IF_DEBUG_BUILD(entry->key.end = NULL);
	return true;
}

Entry* TableGetEntry(Table* table, StringView key)
{
	Entry* entry = table_find_entry(table->entries, table->capacity, key);
	if (entry->key.start == NULL)
		return NULL;
	return entry;
}

void TablePrint(const Table* table)
{
	fputs("============ TABLE ============\n", stdout);
	if (table->count == 0)
		fputs("!EMPTY!", stdout);
	for (size_t i = 0; i < table->capacity; i++)
	{
		if (table->entries[i].key.start == NULL)
			continue;
		printf("%.*s: typeof(%.*s)", (uint32_t)(table->entries[i].key.end - table->entries[i].key.start), table->entries[i].key.start,
			(uint32_t)(table->entries[i].type.name.end - table->entries[i].type.name.start), table->entries[i].type.name.start
		);
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

void table_grow_capacity(Table* table, uint64_t capacity)
{
	Entry* entries = safe_malloc(sizeof(Entry) * capacity);
	for (size_t i = 0; i < capacity; i++)
		entries[i].key.start = NULL;

	for (size_t i = 0; i < table->capacity; i++)
	{
		const Entry* entry = &table->entries[i];
		if (entry->key.start == NULL)
			continue;

		Entry* dest = table_find_entry(entries, capacity, entry->key);
		dest->key = entry->key;
		dest->value = entry->value;
	}

	safe_free(table->entries);
	table->entries = entries;
	table->capacity = capacity;
}

Entry* table_find_entry(Entry* entries, uint64_t capacity, StringView strv)
{
	uint64_t index = hash_strv(strv) % capacity;
	for (;;)
	{
		Entry* entry = &entries[index];
		if (StringViewEqual(entry->key, strv) || entry->key.start == NULL)
			return entry;

		index = (index + 1) % capacity;
	}
}
