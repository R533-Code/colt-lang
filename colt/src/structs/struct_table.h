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
	/// @brief The number of active entries in the VariableTable
	uint64_t count;
	/// @brief The capacity of the VariableTable
	uint64_t capacity;
	/// @brief The pointer to the entries beginning
	Entry* entries;
} VariableTable;

/// @brief Initializes a VariableTable
/// @param table The VariableTable to initialize
void TableInit(VariableTable* table);

/// @brief Frees the resources used by a VariableTable
/// @param table The VariableTable whose resources to free
void TableFree(VariableTable* table);

bool TableGet(VariableTable* table, StringView key, QWORD* value);

bool TableContains(VariableTable* table, StringView key);

bool TableSet(VariableTable* table, StringView strv, QWORD value, Type type);

bool TableDelete(VariableTable* table, StringView key);

/// @brief Prints the content of a VariableTable
/// @param table The VariableTable whose content to print
void TablePrint(const VariableTable* table);

Entry* TableGetEntry(VariableTable* table, StringView key);

/**************************************
IMPLEMENTATION HELPERS
**************************************/

/// @brief Hashes a StringView.
/// Uses FNV-1a as a hash function.
/// @param strv The StringView to hash
/// @return The unsigned int representing the hash
uint64_t hash_strv(StringView strv);

/// @brief Doubles the capacity of the table
/// @param table The VariableTable to modify
/// @param capacity
void table_grow_capacity(VariableTable* table, uint64_t capacity);

Entry* table_find_entry(Entry* entries, uint64_t capacity, StringView strv);

#endif //HG_COLT_STRUCT_TABLE