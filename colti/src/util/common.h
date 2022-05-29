/** @file common.h
* Contains common macros, includes and types used throughout the interpreter.
*/

#ifndef HG_COLTI_COMMON
#define HG_COLTI_COMMON

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "colti_config.h"
#include "memory.h"
#include "console_colors.h"
#include "values/colti_floating_value.h"
#include "values/colti_integer_value.h"

/// @brief Represents the result of an interpretation of a program
typedef enum
{
	INTERPRET_OK, ///< Interpreting was successful
	INTERPRET_COMPILE_ERROR, ///< There was a compilation error
	INTERPRET_RUNTIME_ERROR ///< There was a runtime error
} InterpretResult;

/// @brief Common exit codes to use in place of ints
typedef enum
{
	/// @brief Successful program
	EXIT_NO_FAILURE,
	/// @brief Failed due to an assertion
	EXIT_ASSERTION_FAILURE,
	/// @brief Failed due to an OS-related resource
	EXIT_OS_RESOURCE_FAILURE,
	/// @brief Failed due to an invalid user input
	EXIT_USER_INVALID_INPUT,
} ExitCode;

/// @brief Represents a Byte, which can be type-punned to a more useful type
typedef union
{
	ColtiBool b; ///< Bool member
	ColtiI8 i8; ///< 8-bit signed integer
	ColtiU8 ui8; ///< 8-bit unsigned integer member
} BYTE;

/// @brief Represents a Word (2 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtiBool b; ///< Bool member
	ColtiI8 i8; ///< 8-bit signed integer
	ColtiU8 ui8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/ui8

	ColtiI16 i16; ///< 16-bit signed integer
	ColtiU16 ui16; ///< 16-bit unsigned integer
} WORD;

/// @brief Represents a Double Word (4 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtiBool b; ///< Bool member
	ColtiI8 i8; ///< 8-bit signed integer
	ColtiU8 ui8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/ui8

	ColtiI16 i16; ///< 16-bit signed integer
	ColtiU16 ui16; ///< 16-bit unsigned integer
	WORD word; ///< WORD union, which can be used in place of i16/ui16

	ColtiFloat f; ///< 32-bit float
	ColtiI32 i32; ///< 32-bit signed integer
	ColtiU32 ui32; ///< 32-bit unsigned integer
} DWORD;

/// @brief Represents a Quad Word (8 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtiBool b; ///< Bool member
	ColtiI8 i8; ///< 8-bit signed integer
	ColtiU8 ui8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/ui8

	ColtiI16 i16; ///< 16-bit signed integer
	ColtiU16 ui16; ///< 16-bit unsigned integer
	WORD word; ///< WORD union, which can be used in place of i16/ui16
	
	ColtiFloat f; ///< 32-bit float
	ColtiI32 i32; ///< 32-bit signed integer
	ColtiU32 ui32; ///< 32-bit unsigned integer
	DWORD dword; ///< DWORD union, which can be used if place of f/i32/ui32

	ColtiDouble d; ///< 64-bit float
	ColtiI64 i64; ///< 64-bit signed integer
	ColtiU64 ui64; ///< 64-bit unsigned integer
} QWORD;


/*******************************************
MACRO HELPERS FOR ASSERTION AND ALLOCATIONS
*******************************************/

#ifdef COLTI_DEBUG_BUILD
	#ifdef COLTI_WINDOWS
		/// @brief The current filename, only for debugging purposes (strips path using Windows separator)
		#define COLTI_CURRENT_FILENAME (strrchr("\\" __FILE__, '\\') + 1)
	#else
		/// @brief The current filename, only for debugging purposes
		#define COLTI_CURRENT_FILENAME (strrchr("/" __FILE__, '/') + 1)
	#endif
#else
	/// @brief No-overhead current filename
	#define COLTI_CURRENT_FILENAME  __FILE__
#endif


#ifdef COLTI_DEBUG_BUILD
	/// @brief Terminates the program if 'cond' is not true, and in that case prints error with useful debug info
	#define colti_assert(cond, error) do { \
	if (!(cond)) { \
		printf(CONSOLE_FOREGROUND_BRIGHT_RED "\nAssertion failed from file " CONSOLE_FOREGROUND_BRIGHT_WHITE "\"%s\"" \
			CONSOLE_FOREGROUND_BRIGHT_RED ", at line " CONSOLE_FOREGROUND_BRIGHT_MAGENTA "%d" \
			CONSOLE_FOREGROUND_BRIGHT_RED " in function " CONSOLE_FOREGROUND_BRIGHT_WHITE "\"%s\"" \
			CONSOLE_FOREGROUND_BRIGHT_RED ":\nError: " CONSOLE_FOREGROUND_BRIGHT_CYAN "%s\n" CONSOLE_COLOR_RESET, \
			COLTI_CURRENT_FILENAME, __LINE__, __FUNCTION__, (error)); \
		(void)getc(stdin); \
		exit(EXIT_ASSERTION_FAILURE); \
	} } while (0)
	
	/// @brief Ensures no NULL pointer is returned from a heap allocation
	#define safe_malloc(size)		checked_malloc(size)
	#define safe_free(ptr)			checked_free(ptr)
	
	/// @brief Does 'what' only on Debug configuration
	#define DO_IF_DEBUG_BUILD(what) do { what; } while(0)
#else
	/// @brief Asserts a condition only on Debug configuration
	#define colti_assert(cond, error)
	
	/// @brief Ensures no NULL pointer is returned from a heap allocation
	#define safe_malloc(size)		checked_malloc(size)
	/// @brief Ensures no NULL pointer is passed for deallocation
	#define safe_free(ptr)			checked_free(ptr)

	/// @brief Does 'what' only on Debug configuration
	#define DO_IF_DEBUG_BUILD(what) do {} while(0)
#endif

/// @brief Prints an error and appends a newline.
/// 'format' should be a compile-time known string.
/// Due to how the preprocessor works, always format at least an argument.
#define print_error_format(format, ...)	fprintf(stderr, CONSOLE_FOREGROUND_BRIGHT_RED"Error: "CONSOLE_COLOR_RESET format"\n", __VA_ARGS__)

/// @brief Prints an error and appends a newline.
/// 'str' should be a compile-time known string.
#define print_error_string(str)			fputs(CONSOLE_FOREGROUND_BRIGHT_RED"Error: "CONSOLE_COLOR_RESET str"\n", stderr)

/// @brief Prints a warning and appends a newline.
/// 'format' should be a compile-time known string.
/// Due to how the preprocessor works, always format at least an argument.
#define print_warn_format(format, ...)	printf(CONSOLE_FOREGROUND_BRIGHT_YELLOW"Warning: "CONSOLE_COLOR_RESET format"\n", __VA_ARGS__)

/// @brief Prints a warning and appends a newline.
/// 'str' should be a compile-time known string.
#define print_warn_string(str)			fputs(CONSOLE_FOREGROUND_BRIGHT_YELLOW"Warning: "CONSOLE_COLOR_RESET str"\n", stdout)

#endif //HG_COLTI_COMMON