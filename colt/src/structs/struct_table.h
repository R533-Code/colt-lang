/** @file struct_table.h
* Contains the variable hash-table used for global variables.
* The GlobalTable is responsible of providing a simplified container
* for storing global variable declarations.
*/

#ifndef HG_COLT_STRUCT_TABLE
#define HG_COLT_STRUCT_TABLE

#include "common.h"
#include "type/type.h"
#include "struct_string.h"

/// @brief The maximum occupied capacity before growing the table
#define VARIABLE_TABLE_MAX_LOAD 0.75

/// @brief The maximum occupied capacity before growing the table
#define STRING_TABLE_MAX_LOAD 0.75

/// @brief An entry to a GlobalTable.
/// An GlobalEntry is considered uninitialized if its key->ptr == NULL.
typedef struct
{
	/// @brief The key (which is the name of the variable)
	String key;
	/// @brief The value of the variable
	QWORD value;
	/// @brief The type of the variable
	Type type;
	/// @brief The number of the variable (which is used for offsets)
	uint64_t counter_nb;
} GlobalEntry;

/// @brief An string literal entry to a GlobalTable.
/// An GlobalEntry is considered uninitialized if its key->ptr == NULL.
typedef struct
{
	/// @brief The key which is the actual string
	String key;
	/// @brief The number of the literal (which is used for offsets)
	uint64_t counter_nb;
} StringEntry;

/// @brief A hash-table used for global variables of a program.
/// For an entry of a GlobalTable to be considered uninitialized, its key->ptr is equal to NULL.
typedef struct
{
	/// @brief The number of active variables in the GlobalTable
	uint64_t count;
	/// @brief The capacity of the GlobalTable
	uint64_t capacity;
	/// @brief The pointer to the entries array
	GlobalEntry* entries;	
} GlobalTable;

/// @brief A hash-table used for string literals of a program.
/// For an entry of a StringTable to be considered uninitialized, its key->ptr is equal to NULL.
typedef struct
{
	/// @brief The number of active variables in the GlobalTable
	uint64_t count;
	/// @brief The capacity of the GlobalTable
	uint64_t capacity;
	/// @brief The sum of the size of all the active string length
	uint64_t all_str_size;
	/// @brief The pointer to the string entries array
	StringEntry* str_entries;
} StringTable;

/// @brief Contains a LocalTable, GlobalTable and StringTable
typedef struct
{
	/// @brief The global variables table
	GlobalTable glob_table;
	/// @brief The string literals table
	StringTable str_table;
} ASTTable;

/// @brief Initializes both tables inside an ASTTable
/// @param table The table to initialize
void ASTTableInit(ASTTable* table);

/// @brief Frees resources used by both tables inside a ASTTable
/// @param table The table to modify
void ASTTableFree(ASTTable* table);

/// @brief Initializes a StringTable
/// @param table The StringTable to initialize
void StringTableInit(StringTable* table);

/// @brief Frees the resources used by a StringTable
/// @param table The StringTable whose resources to free
void StringTableFree(StringTable* table);

/// @brief Adds a String to the table
/// @param table The StringTable to add to
/// @param to_add The String to add
void StringTableAdd(StringTable* table, const String* to_add);

/// @brief Initializes a GlobalTable
/// @param table The GlobalTable to initialize
void VariableTableInit(GlobalTable* table);

/// @brief Frees the resources used by a GlobalTable
/// @param table The GlobalTable whose resources to free
void VariableTableFree(GlobalTable* table);

/// @brief Get the value of an entry if it exists.
/// If this function returns True, then it has written the value
/// of an entry to 'value'.
/// @param table The table from which to get the entry
/// @param key The key to the entry
/// @param value Where to write the entry's value if it is found
/// @return True if the entry was found and its value written to 'value'
bool VariableTableGet(GlobalTable* table, StringView key, QWORD* value);

/// @brief Check is a variable is already exists in a GlobalTable
/// @param table The table where to search for
/// @param key The key to check for
/// @return True if an entry already has a key equal to 'key'
bool VariableTableContains(GlobalTable* table, StringView key);

/// @brief Creates an entry if it does not exist or overwrite one
/// @param table The table to modify
/// @param strv The key of the value to write/modify
/// @param value The value to assign
/// @param type The type of the variable
/// @return True if a new entry was created, False if an old entry was overridden
bool VariableTableSet(GlobalTable* table, StringView strv, QWORD value, Type type);

/// @brief Deletes an entry if it exists
/// @param table The table to modify
/// @param key The key of the entry to delete
/// @return True if deletion happened
bool VariableTableDelete(GlobalTable* table, StringView key);

/// @brief Gets an entry if it exists or NULL
/// @param table The table from which to search for the entry
/// @param key The key whose entry to search for
/// @return A pointer to the entry if it exits or NULL
GlobalEntry* VariableTableGetEntry(GlobalTable* table, StringView key);

/**************************************
IMPLEMENTATION HELPERS
**************************************/

/// @brief Hashes a StringView.
/// Uses FNV-1a as a hash function.
/// @param strv The StringView to hash
/// @return The unsigned int representing the hash
uint64_t hash_strv(StringView strv);

/// @brief Grows the capacity of the table
/// @param table The StringTable to modify
/// @param capacity The new capacity
void string_table_grow_capacity(StringTable* table, uint64_t capacity);

/// @brief Find an existing entry or searches for the next empty slot where to write
/// @param entries The pointer to the entries
/// @param capacity The size of the buffer of entries
/// @param strv The key to search for
/// @return A valid pointer to an empty entry or to an existing one with the same key as 'strv'
StringEntry* string_table_find_entry(StringEntry* entries, uint64_t capacity, StringView strv);

/// @brief Grows the capacity of the table
/// @param table The GlobalTable to modify
/// @param capacity The new capacity
void variable_table_grow_capacity(GlobalTable* table, uint64_t capacity);

/// @brief Find an existing entry or searches for the next empty slot where to write
/// @param entries The pointer to the entries
/// @param capacity The size of the buffer of entries
/// @param strv The key to search for
/// @return A valid pointer to an empty entry or to an existing one with the same key as 'strv'
GlobalEntry* variable_table_find_entry(GlobalEntry* entries, uint64_t capacity, StringView strv);

#endif //HG_COLT_STRUCT_TABLE