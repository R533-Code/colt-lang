/** @file struct_string.c
* Contains the definitions of the functions declared in 'struct_string.h'
*/

#include "struct_string.h"

void StringInit(String* str)
{
	str->capacity = STRING_SMALL_BUFFER_OPTIMIZATION;
	str->size = 1;
	str->ptr = str->buffer;
	str->ptr[0] = '\0';
}

void StringFree(String* str)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	colti_assert(str->capacity != 0, "Capacity was 0! Check if the buffer wasn't freed twice!");
	if (str->capacity == STRING_SMALL_BUFFER_OPTIMIZATION)
		return;
	safe_free(str->ptr);

	DO_IF_DEBUG_BUILD(str->size = 0);
	DO_IF_DEBUG_BUILD(str->capacity = 0);
}

void StringPrint(const String* str)
{
	colti_assert(str->size != 0, "A string should at least contain a NUL terminator!");
	printf("%.*s", (int)str->size - 1, str->ptr);
}

uint64_t StringCapacity(const String* str)
{
	return str->capacity;
}

uint64_t StringSize(const String* str)
{
	return str->size;
}

bool StringIsEmpty(const String* str)
{
	colti_assert(str->size != 0, "A string should at least contain a NUL terminator!");
	return str->size == 1;
}

bool StringIsStackAllocated(const String* str)
{
	return str->capacity == STRING_SMALL_BUFFER_OPTIMIZATION;
}

bool StringReplaceChar(String* str, char character, char with)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	for (size_t i = 0; i < str->size; i++)
	{
		if (str->ptr[i] == character)
		{
			str->ptr[i] = with;
			return true;
		}
	}
	return false;
}

uint64_t StringReplaceAllChar(String* str, char character, char with)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	uint64_t nb_of_replace = 0;
	for (size_t i = 0; i < str->size; i++)
	{
		if (str->ptr[i] == character)
		{
			str->ptr[i] = with;
			nb_of_replace++;
		}
	}
	return nb_of_replace;
}

bool StringReplaceString(String* str, const char* what, const char* with)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	colti_assert(what != NULL && with != NULL, "One of the strings passed was NULL!");
	
	size_t what_len = strlen(what);
	size_t with_len = strlen(with);
	
	// if what is "", this means we can just append with at the end
	if (what_len == 0)
	{
		StringAppendString(str, with);
		return true;
	}

	for (size_t i = 0; i < str->size - what_len; i++)
	{
		//as what_len is the length without the NUL terminator
		//we don't need to do any arithmetic to optimize anything
		if (strncmp(what, str->ptr + i, what_len) == 0)
		{
			if (str->size - what_len + with_len > str->capacity)
				impl_string_grow_size(str, with_len - what_len);
			//We shift all the characters after 'what' to the right position:
			//this mean we now have exactly enough characters for 'with'
			// ------------------------------------
			// |            |WHAT| REST OF THE STR|
			// ------------------------------------
			//             i^    ^size - i - length of what
			memmove(str->ptr + i + with_len, str->ptr + i + what_len, str->size - i - what_len);

			//Copy with at the right location
			memcpy(str->ptr + i, with, with_len);
			str->size += with_len - what_len;
			return true;
		}
	}
	return false;
}

uint64_t StringReplaceAllString(String* str, const char* what, const char* with)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	colti_assert(what != NULL && with != NULL, "One of the strings passed was NULL!");

	size_t what_len = strlen(what);
	size_t with_len = strlen(with);

	// if what is "", this means we can just append with at the end
	if (what_len == 0)
	{
		StringAppendString(str, with);
		return 1;
	}

	uint64_t nb_of_replace = 0;
	for (size_t i = 0; i < str->size - what_len; i++)
	{
		//as what_len is the length without the NUL terminator
		//we don't need to do any arithmetic to optimize anything
		if (strncmp(what, str->ptr + i, what_len) == 0)
		{
			if (str->size - what_len + with_len > str->capacity)
				impl_string_grow_size(str, with_len - what_len);
			//We shift all the characters after 'what' to the right position:
			//this mean we now have exactly enough characters for 'with'
			// ------------------------------------
			// |            |WHAT| REST OF THE STR|
			// ------------------------------------
			//             i^    ^size - i - length of what
			memmove(str->ptr + i + with_len, str->ptr + i + what_len, str->size - i - what_len);

			//Copy with at the right location
			memcpy(str->ptr + i, with, with_len);
			str->size += with_len - what_len;
			
			//advance passed the replaced string
			i += with_len;
			nb_of_replace++;
		}
	}
	return nb_of_replace;
}

void StringAppendChar(String* str, char what)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	colti_assert(str->size != 0, "A string should at least contain a NUL terminator!");
	if (str->size == str->capacity)
		impl_string_grow_double(str);
	str->ptr[str->size - 1] = what; //Replaced the old NUL by the character
	str->ptr[str->size++] = '\0'; //Appends a NUL at the end of the string
}

void StringAppendString(String* str, const char* what)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	colti_assert(str->size != 0, "A string should at least contain a NUL terminator!");
	size_t what_len = strlen(what);
	if (str->size + what_len > str->capacity)
		impl_string_grow_size(str, what_len);
	memcpy(str->ptr + str->size - 1, what, what_len); //We overwrite the NUL character by the rest of the string
	str->size += what_len;
	str->ptr[str->size - 1] = '\0';
}

bool StringEqual(const String* lhs, const String* rhs)
{
	colti_assert(lhs->ptr != NULL && rhs->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	if (lhs->size != rhs->size)
		return false;
	//we don't care about comparing the NUL terminator
	return memcmp(lhs->ptr, rhs->ptr, lhs->size - 1) == 0;	
}

bool StringContains(const String* str, const char* what)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	size_t what_size = strlen(what);

	for (size_t i = 0; i < str->size - what_size; i++)
	{
		if (str->ptr[i] == what[0])
		{
			bool found = true;
			for (size_t j = 1; j < what_size; j++)
			{
				if (str->ptr[i + j] != what[j])
				{
					found = false;
					break;
				}
			}
			if (found)
				return found;
			continue;
		}
	}
	return false;
}

void StringFill(String* str, char character)
{
	colti_assert(str->ptr != NULL, "Huge bug: a string's buffer was NULL!");
	memset(str->ptr, character, str->size);
}

void StringClear(String* str)
{
	str->size = 1;
	str->ptr[0] = '\0';
}

void StringReserve(String* str, size_t size)
{
	char* temp = (char*)safe_malloc(str->capacity += size);
	memcpy(temp, str->ptr, str->size);

	if (str->capacity != STRING_SMALL_BUFFER_OPTIMIZATION + size)
		safe_free(str->ptr);
	str->ptr = temp;
}

String StringGetLine()
{
	String str;
	str.ptr = unsafe_string_getline(&str.size, &str.capacity);
	return str;	
}

String StringGetFileContent(const char* path)
{
	FILE* file = fopen(path, "rb"); //Read-binary mode
	if (file == NULL)
	{
		print_error_format("'%s' is not a valid file path!", path);
		exit(EXIT_USER_INVALID_INPUT);
	}
	fseek(file, 0L, SEEK_END);
	size_t file_size = ftell(file); //Get file size
	
	String str;
	str.ptr = safe_malloc(str.capacity = file_size + 1);
	str.size = str.capacity;
	
	rewind(file); //Go back to the beginning of the file
	size_t bytes_read = fread(str.ptr, sizeof(char), file_size, file);
	fclose(file);
	if (bytes_read != file_size)
	{
		print_error_format("Could not read all the content of the file at path '%s'!", path);
		exit(EXIT_OS_RESOURCE_FAILURE);
	}
	str.ptr[str.size - 1] = '\0';
	return str;
}

StringView StringToStringView(const String* str)
{
	StringView strv = { str->ptr, str->ptr + str->size - 1 };
	return strv;
}

void StringViewPrint(const StringView strv)
{
	printf("%.*s", (int)(strv.end - strv.start), strv.start);
}

bool StringViewEqual(StringView lhs, StringView rhs)
{
	if (lhs.end - lhs.start != rhs.end - rhs.start)
		return false;
	return memcmp(lhs.start, rhs.start, lhs.end - lhs.start) == 0;
}

/*****************************************
IMPLEMENTATION HELPERS
*****************************************/

void impl_string_grow_double(String* str)
{
	colti_assert(str->capacity != 0, "Capacity was 0!");
	char* temp = (char*)safe_malloc(str->capacity *= 2);
	
	memcpy(temp, str->ptr, str->size);
	if (str->capacity != STRING_SMALL_BUFFER_OPTIMIZATION * 2)
		safe_free(str->ptr);
	str->ptr = temp;
}

void impl_string_grow_size(String* str, size_t by)
{
	char* temp = (char*)safe_malloc(str->capacity += by);

	memcpy(temp, str->ptr, str->size);
	if (str->capacity - by != STRING_SMALL_BUFFER_OPTIMIZATION)
		safe_free(str->ptr);
	str->ptr = temp;
}

char* unsafe_string_getline(size_t* length, size_t* capacity)
{
	char* str = safe_malloc(10);
	size_t current_char = 0;
	size_t current_capacity = 10;

	for (;;)
	{
		if (current_char == current_capacity)
		{
			char* temp = safe_malloc(current_capacity *= 2);
			memcpy(temp, str, current_char);
			safe_free(str);
			str = temp;
		}
		char gchar = (char)getc(stdin);
		if (gchar != '\n' && gchar != EOF)
		{
			str[current_char++] = gchar;
		}
		else
		{
			str[current_char++] = '\0';
			break;
		}
	}

	*capacity = current_capacity;
	*length = current_char;
	return str;
}