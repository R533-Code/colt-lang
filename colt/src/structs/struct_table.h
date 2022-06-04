#ifndef HG_COLT_STRUCT_TABLE
#define HG_COLT_STRUCT_TABLE

#include "common.h"
#include "lang/type.h"
#include "struct_string.h"

/// @brief The maximum occupied capacity before growing the table
#define TABLE_MAX_LOAD 0.75

typedef struct
{
	String key;
	QWORD value;
	Type type;
	uint64_t counter_nb;
} Entry;

typedef struct
{
	/// @brief A counter incremented at each pushed item
	uint64_t counter;
	/// @brief The number of active entries in the Table
	uint64_t count;
	/// @brief The capacity of the Table
	uint64_t capacity;
	/// @brief The pointer to the entries beginning
	Entry* entries;
} Table;

/// @brief Initializes a Table
/// @param table The Table to initialize
void TableInit(Table* table);

/// @brief Frees the resources used by a Table
/// @param table The Table whose resources to free
void TableFree(Table* table);

bool TableGet(Table* table, StringView key, QWORD* value);

bool TableContains(Table* table, StringView key);

bool TableSet(Table* table, StringView strv, QWORD value, Type type);

bool TableDelete(Table* table, StringView key);

/// @brief Prints the content of a Table
/// @param table The Table whose content to print
void TablePrint(const Table* table);

Entry* TableGetEntry(Table* table, StringView key);

/**************************************
IMPLEMENTATION HELPERS
**************************************/

/// @brief Hashes a StringView.
/// Uses FNV-1a as a hash function.
/// @param strv The StringView to hash
/// @return The unsigned int representing the hash
uint64_t hash_strv(StringView strv);

/// @brief Doubles the capacity of the table
/// @param table The Table to modify
/// @param capacity
void table_grow_capacity(Table* table, uint64_t capacity);

Entry* table_find_entry(Entry* entries, uint64_t capacity, StringView strv);

#endif //HG_COLT_STRUCT_TABLE