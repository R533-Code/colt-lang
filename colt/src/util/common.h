/** @file common.h
* Contains common macros, includes and types used throughout the interpreter.
*/

#ifndef HG_COLT_COMMON
#define HG_COLT_COMMON

#include "colt_config.h"

#ifdef COLT_MSVC
	//We don't want warnings when using 'fopen'
	#define _CRT_SECURE_NO_WARNINGS
#endif

//DEBUGING UTILITIES
#if defined(COLT_MSVC) && defined(COLT_DEBUG_BUILD)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h> //Contains _CrtDumpMemoryLeaks()
#define DUMP_MEMORY_LEAKS() do { \
		_CrtSetReportMode(_CRT_WARN,_CRTDBG_MODE_FILE); \
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT); \
		fputs("\n\n"CONSOLE_COLOR_REVERSE"Checking for memory leaks..."CONSOLE_COLOR_RESET"\n", stdout); \
		if (_CrtDumpMemoryLeaks() == 0) \
			printf("No memory leaks.\n"); } while (0)
#else
	/// @brief On Windows and Debug build, using Visual Studio, this will show memory leaks (if there are any)
#define DUMP_MEMORY_LEAKS() do {} while(0)
#endif


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
	ColtBool_t b; ///< Bool member
	ColtI8_t i8; ///< 8-bit signed integer
	ColtU8_t u8; ///< 8-bit unsigned integer member
} BYTE;

/// @brief Represents a Word (2 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtBool_t b; ///< Bool member
	ColtI8_t i8; ///< 8-bit signed integer
	ColtU8_t u8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/u8

	ColtI16_t i16; ///< 16-bit signed integer
	ColtU16_t u16; ///< 16-bit unsigned integer
} WORD;

/// @brief Represents a Double Word (4 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtBool_t b; ///< Bool member
	ColtI8_t i8; ///< 8-bit signed integer
	ColtU8_t u8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/u8

	ColtI16_t i16; ///< 16-bit signed integer
	ColtU16_t u16; ///< 16-bit unsigned integer
	WORD word; ///< WORD union, which can be used in place of i16/u16

	ColtFloat_t f; ///< 32-bit float
	ColtI32_t i32; ///< 32-bit signed integer
	ColtU32_t u32; ///< 32-bit unsigned integer
} DWORD;

/// @brief Represents a Quad Word (8 bytes), which can be type-punned to a more useful type
typedef union
{
	ColtBool_t b; ///< Bool member
	ColtI8_t i8; ///< 8-bit signed integer
	ColtU8_t u8; ///< 8-bit unsigned integer member
	BYTE byte; ///< BYTE union, which can be used in place of b/i8/u8

	ColtI16_t i16; ///< 16-bit signed integer
	ColtU16_t u16; ///< 16-bit unsigned integer
	WORD word; ///< WORD union, which can be used in place of i16/u16
	
	ColtFloat_t f; ///< 32-bit float
	ColtI32_t i32; ///< 32-bit signed integer
	ColtU32_t u32; ///< 32-bit unsigned integer
	DWORD dword; ///< DWORD union, which can be used if place of f/i32/u32

	ColtDouble_t d; ///< 64-bit float
	ColtI64_t i64; ///< 64-bit signed integer
	ColtU64_t u64; ///< 64-bit unsigned integer
} QWORD;


/*******************************************
MACRO HELPERS FOR ASSERTION AND ALLOCATIONS
*******************************************/

#ifdef COLT_DEBUG_BUILD
	#ifdef COLT_WINDOWS
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


#ifdef COLT_DEBUG_BUILD
	/// @brief Terminates the program if 'cond' is not true, and in that case prints error with useful debug info
	#define colt_assert(cond, error) do { \
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
	#define colt_assert(cond, error)
	
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

#endif //HG_COLT_COMMON