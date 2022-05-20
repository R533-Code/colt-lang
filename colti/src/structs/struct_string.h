/** @file struct_string.h
* A String struct helper for dealing with strings.
* The string struct uses a small-buffer optimization.
* Common operations, like getting content from a file, or replacing a string are a lot easier
* using the provided functions.
* This header also contains the StringView struct, which is a lightweight
* struct representing a non-owning view over an array of characters.
*/

#ifndef HG_COLTI_STRUCT_STRING
#define HG_COLTI_STRUCT_STRING

#include "common.h"

/// @brief The stack preallocated buffer of Strings
#define STRING_SMALL_BUFFER_OPTIMIZATION 32

/// @brief An owning array of characters that is always NUL terminated
typedef struct
{
	/// @brief The capacity of the String
	uint64_t capacity;
	/// @brief The byte-size of the String, including the NUL terminator
	uint64_t size; 
	/// @brief The pointer to the beginning of the array of characters
	char* ptr;
	/// @brief Stack-buffer for optimization of small Strings
	char buffer[STRING_SMALL_BUFFER_OPTIMIZATION];
} String;


/// @brief Initializes a string
/// @param str The string to initialize
void StringInit(String* str);

/// @brief Frees the resources used by a string
/// @param str The string to modify
void StringFree(String* str);

/// @brief Prints the content of the string to stdout
/// @param str The string to print
void StringPrint(const String* str);

/// @brief Returns the current capacity of a string
/// @param str The string for which to check
/// @return The capacity of the string
uint64_t StringCapacity(const String* str);

/// @brief Returns the size of a string
/// @param str The string for which to check
/// @return The size of the string
uint64_t StringSize(const String* str);

/// @brief Checks if a string is empty or not
/// @param str True if empty
bool StringIsEmpty(const String* str);

/// @brief Checks if a string is using its stack-allocated buffer
/// @param str The string for which to check
bool StringIsStackAllocated(const String* str);

/// @brief Replaces the first instance of 'character' with 'with'
/// @param str The string to modify
/// @param character The character for which to search
/// @param with The character to replace with
/// @return True if one 'character' was replaced with 'with'
bool StringReplaceChar(String* str, char character, char with);

/// @brief Replaces all instances of 'character' with 'with'
/// @param str The string to modify
/// @param character The character for which to search
/// @param with The character to replace with
/// @return The number of replacement that happened
uint64_t StringReplaceAllChar(String* str, char character, char with);

/// @brief Replaces the first instance of 'what' with 'with'
/// @param str The string to modify
/// @param what The string to search for
/// @param with The string to replace with
/// @return True if the 'what' was replaced by 'with'
bool StringReplaceString(String* str, const char* what, const char* with);

/// @brief Replaces all instances of 'what' with 'with'
/// @param str The string to modify
/// @param what The string for which to search
/// @param with The string to replace with
/// @return The number of replacement that happened
uint64_t StringReplaceAllString(String* str, const char* what, const char* with);

/// @brief Appends 'what' to the end of 'str'
/// @param str The string to modify
/// @param what The character to append
void StringAppendChar(String* str, char what);

/// @brief Appends 'what' to the end of 'str'
/// @param str The string to modify
/// @param what The NUL terminated string to append
void StringAppendString(String* str, const char* what);

/// @brief Check if two strings are equal.
/// More optimized than `strcmp` as we check for if the size of the strings are equal.
/// @param lhs The left hand side
/// @param rhs The right hand side
/// @return True if the strings are equal
bool StringEqual(const String* lhs, const String* rhs);

/// @brief Check if a String contains 'what'
/// @param str The String in which to check
/// @param what The string to search for
/// @return True if the 'str' contains 'what'
bool StringContains(const String* str, const char* what);

/// @brief Fills a string with the specified character
/// @param str The string to modify
/// @param character The character with which to fill the string
void StringFill(String* str, char character);

/// @brief Clears a String's content.
/// Sets the size of the String to 0, and sets its first character to NUL.
/// @param str The string to modify
void StringClear(String* str);

/// @brief Reserves 'size' MORE bytes for the string capacity.
/// @param str The string to modify
/// @param size The number of bytes to add to the capacity
void StringReserve(String* str, size_t size);

/// @brief Asks the user for input, and returns it as a string
/// @return The string containing the input
String StringGetLine();

/// @brief Reads all the content of a file and writes to a string.
/// Exits if the file cannot be opened, or not all its content can be read.
/// @param path The path to the file
/// @return The string containing the file data
String StringGetFileContent(const char* path);

/// @brief A non-owning view over an array of characters
typedef struct
{
	/// @brief Pointer to the beginning of the character array
	const char* start;
	/// @brief Pointer to the end of the character array
	const char* end;
} StringView;

/// @brief Returns a string view over a string, without its NUL terminator
/// @param str The string from which to extract the view
/// @return A view over the whole 'str' - NUL
StringView StringToStringView(const String* str);

/// @brief Prints a string view to stdout
/// @param strv The view to print
void StringViewPrint(const StringView strv);

/// @brief Check if 2 string views are equal
/// @param lhs The left hand side
/// @param rhs The right hand side
/// @return True if the content pointed by the views is the same
bool StringViewEqual(StringView lhs, StringView rhs);

/*****************************************
IMPLEMENTATION HELPERS
*****************************************/

/// @brief Doubles the capacity of a string
/// @param str The string to modify
void impl_string_grow_double(String* str);

/// @brief Augments the capacity of a string by 'by' bytes
/// @param str The string to modify
/// @param by The number of bytes to add to the capacity
void impl_string_grow_size(String* str, size_t by);

/// @brief Gets a heap-allocated line from 'stdin'
/// @param length The pointer to where to write the size
/// @param capacity The pointer to where to write the capacity
/// @return The line representing the user input
char* unsafe_string_getline(size_t* length, size_t* capacity);

#endif //HG_COLTI_STRUCT_STRING